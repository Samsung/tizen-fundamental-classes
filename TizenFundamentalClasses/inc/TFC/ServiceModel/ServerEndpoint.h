/*
 * ServerEndpoint.h
 *
 *  Created on: Dec 28, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_SERVERENDPOINT_H_
#define TFC_SERVICEMODEL_SERVERENDPOINT_H_

#include "TFC/Core/Introspect.h"

#include <unordered_map>
#include <functional>

namespace TFC {
namespace ServiceModel {

template<typename TEndpoint>
class IServerObject
{
public:
	typedef typename TEndpoint::Channel Channel;
	typedef typename Channel::InterfaceDefinition InterfaceDefinition;
	typedef typename TEndpoint::PackType PackType;

	virtual InterfaceDefinition const& GetInterfaceDefinition() = 0;
	virtual PackType Invoke(std::string const& function, PackType param) = 0;

	virtual ~IServerObject() { }
};

template<typename TEndpoint, typename T>
class ServerObjectManager
{
public:
	typedef typename TEndpoint::Channel Channel;
	typedef typename Channel::InterfaceDefinition InterfaceDefinition;
	typedef typename TEndpoint::PackType PackType;


};

template<typename TEndpoint, typename T>
class ServerObject : public T, public IServerObject<TEndpoint>
{
private:
	typedef typename TEndpoint::Channel Channel;
	typedef typename Channel::Deserializer 		Deserializer;
	typedef typename Channel::Serializer 		Serializer;

	typedef void (T::* PointerToMemberFunctionType)();

	struct FunctionDelegate
	{
		PointerToMemberFunctionType targetFunc;
		typename Channel::PackType (*delegateFunc)(T* instance, PointerToMemberFunctionType targetFunc, typename Channel::PackType p);
	};

	std::string objectPath;

	static typename Channel::InterfaceDefinition definition;
	static std::unordered_map<std::string, FunctionDelegate> functionMap;
	static bool initialized;

protected:
	ServerObject(void(*initializerFunc)())
	{
		if(!initialized)
		{
			definition.SetInterfaceName(GetInterfaceName(nullptr, typeid(T)));
			initializerFunc();
			initialized = true;
		}
	}

	template<typename TFuncPtr, typename = typename Core::Introspect::MemberFunction<TFuncPtr>::ReturnType>
	struct InvokerSelector
	{
		static auto Func(T* instance, PointerToMemberFunctionType targetFunc, typename Channel::PackType p)
			-> typename Channel::PackType
		{
			auto targetFuncCasted = reinterpret_cast<TFuncPtr>(targetFunc);

			auto params = ParameterDeserializer<Deserializer, TFuncPtr>::Deserialize(p, false);
			typedef DelayedInvoker<TFuncPtr> Invoker;
			auto var = Invoker::Invoke(instance, targetFuncCasted, params);
			typedef ObjectSerializer<Serializer, typename Core::Introspect::MemberFunction<TFuncPtr>::ReturnType> Serializer;
			return Serializer::Serialize(var);
		}
	};

	template<typename TFuncPtr>
	struct InvokerSelector<TFuncPtr, void>
	{
		static auto Func(T* instance, PointerToMemberFunctionType targetFunc, typename Channel::PackType p)
					-> typename Channel::PackType
		{
			auto targetFuncCasted = reinterpret_cast<TFuncPtr>(targetFunc);
			auto params = ParameterDeserializer<Deserializer, TFuncPtr>::Deserialize(p, false);

			typedef DelayedInvoker<TFuncPtr> Invoker;
			Invoker::Invoke(instance, targetFuncCasted, params);

			typedef ObjectSerializer<Serializer, typename Core::Introspect::MemberFunction<TFuncPtr>::ReturnType> Serializer;
			return Serializer::Serialize();
		}
	};

	template<typename TFuncPtr>
	static void RegisterFunction(TFuncPtr ptr)
	{
		auto& typeDescription = TypeInfo<T>::typeDescription;

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

public:
	virtual typename Channel::InterfaceDefinition const& GetInterfaceDefinition() override
	{
		return definition;
	}


};

}}



#endif /* TFC_SERVICEMODEL_SERVERENDPOINT_H_ */
