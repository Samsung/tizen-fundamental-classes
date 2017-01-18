/*
 * ServerEndpoint.h
 *
 *  Created on: Dec 28, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_SERVERENDPOINT_H_
#define TFC_SERVICEMODEL_SERVERENDPOINT_H_

#include "TFC/Core/Introspect.h"
#include "TFC/ServiceModel/InterfaceInspector.h"
#include "TFC/ServiceModel/Reflection.h"

#include "TFC/Serialization/ObjectSerializer.h"
#include "TFC/Serialization/ParameterSerializer.h"

#include <map>
#include <unordered_map>
#include <functional>
#include <memory>

namespace TFC {
namespace ServiceModel {

template<typename TChannel>
class IServerObject
{
public:
	typedef typename TChannel::InterfaceDefinition InterfaceDefinition;
	typedef typename TChannel::SerializedType SerializedType;

	struct ServerObjectInvoker
	{
		virtual SerializedType Invoke(std::string const&, SerializedType) = 0;
	};

	struct InterfaceEntry
	{
		std::unique_ptr<ServerObjectInvoker> invoker;
		InterfaceDefinition& definition;

		InterfaceEntry(ServerObjectInvoker* ptr, InterfaceDefinition& def) :
			invoker(ptr), definition(def)
		{
		}
	};

private:
	std::string name;
	std::map<std::string, InterfaceEntry> ifaceVtable;

protected:
	void RegisterInterface(std::string interfaceName, InterfaceEntry ifaceInfo)
	{
		ifaceVtable.emplace(interfaceName, std::move(ifaceInfo));
	}

public:
	IServerObject() { this->name = GetInterfaceName(nullptr, typeid(this)); }
	virtual ~IServerObject() { }

	std::vector<InterfaceDefinition*> GetInterfaceList()
	{
		std::vector<InterfaceDefinition*> ifaceRet;

		for(auto& obj : ifaceVtable)
		{
			ifaceRet.push_back(&(obj.second.definition));
		}

		return ifaceRet;
	}

	SerializedType Invoke(std::string const& ifaceName, std::string const& funcName, SerializedType param)
	{
		auto iter = ifaceVtable.find(ifaceName);

		if(iter == ifaceVtable.end())
			throw TFCException("Requested interface is not found");

		InterfaceEntry& iface = iter->second;

		return iface.invoker->Invoke(funcName, param);
	}

	void SetName(std::string&& name) { this->name = std::move(name); }
	void SetName(std::string const& name) { this->name = name; }
	std::string const& GetName() { return this->name; }
};

template<typename TEndpoint, typename T>
class ServerObject : public T, public virtual IServerObject<typename TEndpoint::Channel>
{
private:
	typedef typename TEndpoint::Channel Channel;
	typedef typename Channel::Deserializer 		Deserializer;
	typedef typename Channel::Serializer 		Serializer;

	typedef void (T::* PointerToMemberFunctionType)();

	struct FunctionDelegate
	{
		PointerToMemberFunctionType targetFunc;
		typename Channel::SerializedType (*delegateFunc)(T* instance, PointerToMemberFunctionType targetFunc, typename Channel::SerializedType p);
	};

	std::string objectPath;

	static typename Channel::InterfaceDefinition definition;
	static std::unordered_map<std::string, FunctionDelegate> functionMap;
	static bool initialized;

	class ServerObjectInvoker : public IServerObject<typename TEndpoint::Channel>::ServerObjectInvoker
	{
	private:
		T* thiz;

		ServerObjectInvoker(T* thizPtr) : thiz(thizPtr) { }
	public:
		virtual typename Channel::SerializedType Invoke(std::string const& funcName, typename Channel::SerializedType params) override
		{
			auto& funcDefinition = functionMap[funcName];
			return funcDefinition.delegateFunc(thiz, funcDefinition.targetFunc, params);
		}

		friend class ServerObject<TEndpoint, T>;
	};

protected:
	ServerObject()
	{
		auto ifaceName = GetInterfaceName(TEndpoint::interfacePrefix, typeid(T));
		if(!initialized)
		{
			definition.SetInterfaceName(ifaceName);
			InitializeInterface();
			initialized = true;
		}

		IServerObject<typename TEndpoint::Channel>::RegisterInterface(ifaceName, { new ServerObjectInvoker(this), definition });
	}

	template<typename TFuncPtr, typename = typename Core::Introspect::MemberFunction<TFuncPtr>::ReturnType>
	struct InvokerSelector
	{
		static auto Func(T* instance, PointerToMemberFunctionType targetFunc, typename Channel::SerializedType p)
			-> typename Channel::SerializedType
		{
			auto targetFuncCasted = reinterpret_cast<TFuncPtr>(targetFunc);

			auto params = Serialization::ParameterDeserializer<Deserializer, TFuncPtr>::Deserialize(p, false);
			typedef DelayedInvoker<TFuncPtr> Invoker;
			auto var = Invoker::Invoke(instance, targetFuncCasted, params);
			typedef Serialization::ObjectSerializer<Serializer, typename Core::Introspect::MemberFunction<TFuncPtr>::ReturnType> Serializer;
			return Serializer::Serialize(var);
		}
	};

	template<typename TFuncPtr>
	struct InvokerSelector<TFuncPtr, void>
	{
		static auto Func(T* instance, PointerToMemberFunctionType targetFunc, typename Channel::SerializedType p)
					-> typename Channel::SerializedType
		{
			auto targetFuncCasted = reinterpret_cast<TFuncPtr>(targetFunc);
			auto params = Serialization::ParameterDeserializer<Deserializer, TFuncPtr>::Deserialize(p, false);

			typedef DelayedInvoker<TFuncPtr> Invoker;
			Invoker::Invoke(instance, targetFuncCasted, params);

			typedef Serialization::ObjectSerializer<Serializer, typename Core::Introspect::MemberFunction<TFuncPtr>::ReturnType> Serializer;
			return Serializer::Serialize();
		}
	};

	static void InitializeInterface();

	template<typename TFuncPtr>
	static void RegisterFunction(TFuncPtr ptr, char const* name)
	{
		functionMap.emplace(
			std::make_pair<std::string, FunctionDelegate>(
				name,
				{
					reinterpret_cast<PointerToMemberFunctionType>(ptr),
					InvokerSelector<TFuncPtr>::Func
				}
		));

		definition.RegisterFunction(ptr);
	}


public:
	virtual ~ServerObject() { }
};

template<typename TEndpoint, typename T>
bool ServerObject<TEndpoint, T>::initialized = false;

template<typename TEndpoint, typename T>
typename ServerObject<TEndpoint, T>::Channel::InterfaceDefinition ServerObject<TEndpoint, T>::definition;

template<typename TEndpoint, typename T>
std::unordered_map<std::string, typename ServerObject<TEndpoint, T>::FunctionDelegate> ServerObject<TEndpoint, T>::functionMap;

template<typename TEndpoint>
class IServerObjectManager
{
public:
	typedef typename TEndpoint::Channel Channel;
	typedef typename Channel::InterfaceDefinition InterfaceDefinition;
	typedef typename TEndpoint::SerializedType SerializedType;

	virtual InterfaceDefinition const& GetInterfaceDefinition() = 0;
	virtual SerializedType Invoke(int id, std::string const& function, SerializedType param) = 0;

	virtual ~IServerObjectManager() { }
};

template<typename TEndpoint, typename T>
class ServerObjectManager : public IServerObjectManager<TEndpoint>
{
public:
	typedef typename TEndpoint::Channel Channel;
	typedef typename Channel::InterfaceDefinition InterfaceDefinition;
	typedef typename TEndpoint::SerializedType SerializedType;

protected:

private:
	struct ManagedObjectInfo
	{
		ServerObject<TEndpoint, T>* instance;
		long long lastAccessed;
	};

	std::map<int, ManagedObjectInfo> objectDictionary;
};

}}



#endif /* TFC_SERVICEMODEL_SERVERENDPOINT_H_ */
