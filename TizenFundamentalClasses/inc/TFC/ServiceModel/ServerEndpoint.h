/*
 * ServerEndpoint.h
 *
 *  Created on: Dec 28, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_SERVERENDPOINT_H_
#define TFC_SERVICEMODEL_SERVERENDPOINT_H_

#include "TFC/Core/Introspect.h"
#include "TFC/Core/Invocation.h"
#include "TFC/Core/Reflection.h"

#include "TFC/Serialization/ObjectSerializer.h"
#include "TFC/Serialization/ParameterSerializer.h"

#include <map>
#include <unordered_map>
#include <functional>
#include <memory>

namespace TFC {
namespace ServiceModel {

template<typename TChannel>
class IServerObject : public EventEmitterClass<IServerObject<TChannel>>
{
public:
	template<typename TArg>
	using Event = typename EventEmitterClass<IServerObject<TChannel>>::template Event<TArg>;

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

	struct EventEmissionInfo
	{
		std::string const& eventName;
		SerializedType const& eventArgument;
	};

	Event<EventEmissionInfo const&> eventEventRaised;

private:
	std::string name;
	std::map<std::string, InterfaceEntry> ifaceVtable;

protected:
	void RegisterInterface(std::string interfaceName, InterfaceEntry ifaceInfo)
	{
		ifaceVtable.emplace(interfaceName, std::move(ifaceInfo));
	}

	void NotifyEventRaised(std::string const& eventName, SerializedType eventArg)
	{
		eventEventRaised(this, { eventName, eventArg });
	}

public:
	IServerObject() { this->name = Core::GetInterfaceName(nullptr, typeid(this)); }


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
	typedef int (T::* PointerToMemberType);

	struct FunctionDelegate
	{
		PointerToMemberFunctionType targetFunc;
		typename Channel::SerializedType (*delegateFunc)(T* instance, PointerToMemberFunctionType targetFunc, typename Channel::SerializedType p);
	};

	struct EventRegistration
	{
		std::string eventName;
		PointerToMemberType targetEvent;
		ObjectClass* (* registerEventFunc)(ServerObject<TEndpoint, T>&, EventRegistration const&);

		ObjectClass* RegisterEvent(ServerObject<TEndpoint, T>& obj)
		{
			return registerEventFunc(obj, *this);
		}
	};

	std::string objectPath;

	static typename Channel::InterfaceDefinition definition;
	static std::unordered_map<std::string, FunctionDelegate> functionMap;
	static std::unordered_map<std::string, EventRegistration> eventMap;
	static bool initialized;

	std::vector<std::unique_ptr<ObjectClass>> eventClosure;

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
		auto ifaceName = Core::GetInterfaceName(TEndpoint::interfacePrefix, typeid(T));
		if(!initialized)
		{
			definition.SetInterfaceName(ifaceName);
			InitializeInterface();
			initialized = true;
		}

		IServerObject<typename TEndpoint::Channel>::RegisterInterface(ifaceName, { new ServerObjectInvoker(this), definition });

		for(auto& eventReg : eventMap)
		{
			eventClosure.push_back(std::unique_ptr<ObjectClass> { eventReg.second.RegisterEvent(*this) });
		}
	}

	template<typename TFuncPtr, typename = typename Core::Introspect::MemberFunction<TFuncPtr>::ReturnType>
	struct InvokerSelector
	{
		static auto Func(T* instance, PointerToMemberFunctionType targetFunc, typename Channel::SerializedType p)
			-> typename Channel::SerializedType
		{
			auto targetFuncCasted = reinterpret_cast<TFuncPtr>(targetFunc);

			auto params = Serialization::ParameterDeserializer<Deserializer, TFuncPtr>::Deserialize(p, false);
			typedef Core::DelayedInvoker<TFuncPtr> Invoker;
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

			typedef Core::DelayedInvoker<TFuncPtr> Invoker;
			Invoker::Invoke(instance, targetFuncCasted, params);

			typedef Serialization::ObjectSerializer<Serializer, typename Core::Introspect::MemberFunction<TFuncPtr>::ReturnType> Serializer;
			return Serializer::Serialize();
		}
	};

	template<typename>
	friend struct EventHandlerClosure;

	template<typename TArg>
	struct EventHandlerClosure : ObjectClass
	{
		ServerObject<TEndpoint, T>& serverRef;
		EventRegistration const& eventRef;

		EventHandlerClosure(ServerObject<TEndpoint, T>& serverRef, EventRegistration const& eventRef) :
			serverRef(serverRef), eventRef(eventRef)
		{
			auto targetEvent = reinterpret_cast<Core::EventObject<T*, TArg> T::*>(eventRef.targetEvent);

			(serverRef.*targetEvent) += EventHandler(EventHandlerClosure::EventHandlerFunc);
		}

		void EventHandlerFunc(T* source, TArg value)
		{
			typedef Serialization::ObjectSerializer<Serializer, TArg> Serializer;
			auto serializedArg = Serializer::Serialize(value);

			serverRef.NotifyEventRaised(eventRef.eventName, serializedArg);
		}

		~EventHandlerClosure()
		{

		}
	};



	template<typename TArg>
	static ObjectClass* RegisterEventHandler(ServerObject<TEndpoint, T>& server, EventRegistration const& eventName)
	{
		return new EventHandlerClosure<TArg> { server, eventName };
	}

	static void InitializeInterface();

	template<typename TFuncPtr>
	static void RegisterFunction(TFuncPtr ptr, char const* name)
	{
		auto& typeDescription = Core::TypeInfo<T>::typeDescription;

		functionMap.emplace(
			std::make_pair<std::string, FunctionDelegate>(
				typeDescription.GetFunctionNameByPointer(ptr),
				{
					reinterpret_cast<PointerToMemberFunctionType>(ptr),
					InvokerSelector<TFuncPtr>::Func
				}
		));

		definition.RegisterFunction(ptr);
	}

	template<typename TArg>
	static void RegisterEvent(TFC::Core::EventObject<T*, TArg> T::* eventPtr, char const* name)
	{
		dlog_print(DLOG_DEBUG, "RPC-Test", "RegisterEvent");

		auto& typeDescription = Core::TypeInfo<T>::typeDescription;

		auto eventName = typeDescription.GetEventNameByPointer(eventPtr);
		eventMap.emplace(
			std::make_pair<std::string, EventRegistration>(
				eventName,
				{
						eventName,
						reinterpret_cast<PointerToMemberType>(eventPtr),
						RegisterEventHandler<TArg>
				}
		));


		definition.RegisterEvent(eventPtr);
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

template<typename TEndpoint, typename T>
std::unordered_map<std::string, typename ServerObject<TEndpoint, T>::EventRegistration> ServerObject<TEndpoint, T>::eventMap;

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
