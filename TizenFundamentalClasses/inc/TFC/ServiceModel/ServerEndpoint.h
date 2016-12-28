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

template<typename TEndpoint, typename T>
class ServerEndpoint : public T
{
private:
	typedef typename TEndpoint::Channel Channel;
	typedef typename Channel::Deserializer Deserializer;
	typedef typename Channel::Serializer Serializer;
	typedef std::function<typename Channel::PackType(T*, typename Channel::PackType)> FunctionDelegate;

	std::string objectPath;
	typename Channel::Server endpoint;

	static std::unordered_map<std::string, FunctionDelegate> functionMap;
	static bool initialized;

	template<typename TFuncPtr>
	static void RegisterFunction(TFuncPtr ptr)
	{

		auto lambda = [ptr] (T* instance, typename Channel::PackType p) {
			auto params = ParameterDeserializer<Deserializer, TFuncPtr>::Deserialize(p, false);
			typedef DelayedInvoker<TFuncPtr> Invoker;
			auto var = Invoker::Invoke(instance, ptr, params);

			typedef ObjectSerializer<Serializer, typename Core::Introspect::MemberFunction<TFuncPtr>::ReturnType> Serializer;
			return Serializer::Serialize(var);
		};

		auto& typeInfo = TypeMetadata<T>::typeInfo;

		functionMap.emplace(std::make_pair(typeInfo.GetFunctionNameByPointer(ptr), std::move(lambda)));
	}

protected:
	ServerEndpoint(void(*initializerFunc)())
	{
		if(!initialized)
		{
			initializerFunc();
			initialized = true;
		}
	}
};

}}



#endif /* TFC_SERVICEMODEL_SERVERENDPOINT_H_ */
