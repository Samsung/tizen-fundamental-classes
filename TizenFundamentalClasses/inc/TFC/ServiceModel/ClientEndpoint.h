/*
 * ClientEndpoint.h
 *
 *  Created on: Dec 27, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_CLIENTENDPOINT_H_
#define TFC_SERVICEMODEL_CLIENTENDPOINT_H_

#include "TFC/Core.h"
#include "TFC/Core/Introspect.h"
#include "TFC/Core/Invocation.h"
#include "TFC/Core/Reflection.h"

#include "TFC/Serialization/ObjectSerializer.h"
#include "TFC/Serialization/ParameterSerializer.h"

#include <dlog.h>
#include <string>
#include <typeinfo>

namespace TFC {
namespace ServiceModel {


template<typename TEndpoint>
struct InterfacePrefixInspector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::interfacePrefix), char const*>::type> static True  TestFunc(decltype(T::interfacePrefix) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TEndpoint>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TEndpoint>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::interfacePrefix; };

	static constexpr auto value = Extractor<>::value;
};

template<typename T>
struct EventEmissionInfo
{
	char const* eventName;
	T eventArg;
};

template<typename TEndpoint, typename T>
class ClientEndpoint : public T
{
private:
	std::string objectPath;
	typedef typename TEndpoint::Channel Channel;
	typename Channel::Client endpoint;

	struct EventDelegate
	{
		void* event;
		void (*raiseFunc)(T* thiz, void* event, typename Channel::SerializedType data);
	};

	std::map<std::string, EventDelegate> eventMap;

	template<typename TMemPtr, typename... TArgs>
	auto InvokeInternal(TMemPtr ptr, TArgs... args)
		-> typename TFC::Core::Introspect::MemberFunction<TMemPtr>::ReturnType
	{
		typedef Serialization::ParameterSerializer<typename Channel::Serializer, TMemPtr> Serializer;
		typedef Serialization::ObjectDeserializer<typename Channel::Deserializer, typename TFC::Core::Introspect::MemberFunction<TMemPtr>::ReturnType> ReturnTypeDeserializer;

		auto& typeDescription = Core::TypeInfo<T>::typeDescription;

		auto serialized = Serializer::Serialize(args...);
		return ReturnTypeDeserializer::Deserialize(endpoint.RemoteCall(typeDescription.GetFunctionNameByPointer(ptr), serialized));
	}

	void EventReceived(typename Channel::Client* sender, EventEmissionInfo<typename Channel::SerializedType> const& info)
	{
		auto data = eventMap.find(info.eventName);
		if(data != eventMap.end())
			data->second.raiseFunc(this, data->second.event, info.eventArg);
	}

	template<typename TArg>
	static void RaiseFunction(T* thiz, void* eventPtr, typename Channel::SerializedType dataPtr)
	{
		auto event = static_cast<TFC::Core::EventObject<T*, TArg>*>(eventPtr);
		typename Channel::Deserializer deser(dataPtr);
		auto data = Serialization::GenericDeserializer<typename Channel::Deserializer, TArg>::Deserialize(deser);
		(*event)(thiz, data);
	}


protected:

	template<typename TMemPtr, typename... TArgs>
	auto Invoke(TMemPtr ptr, TArgs... param)
		-> typename TFC::Core::Introspect::MemberFunction<TMemPtr>::ReturnType
	{
		return InvokeInternal<TMemPtr>(ptr, param...);
	}

	ClientEndpoint(char const* objectPath) :
		endpoint(TEndpoint::configuration, objectPath, Core::GetInterfaceName(InterfacePrefixInspector<TEndpoint>::value, typeid(T)).c_str())
	{
		typedef ClientEndpoint<TEndpoint, T> EP;
		endpoint.eventEventReceived += EventHandler(EP::EventReceived);
	}

	template<typename TArg>
	void RegisterEvent(TFC::Core::EventObject<T*, TArg> T::* eventPtr)
	{
		auto& typeInfo = Core::TypeInfo<T>::typeDescription;
		char const* eventName = typeInfo.GetEventNameByPointer(eventPtr);

		eventMap.emplace(std::make_pair(std::string(eventName), EventDelegate {
			&(this->*eventPtr),
			ClientEndpoint<TEndpoint, T>::RaiseFunction<TArg>
		}));
	}

public:

};



}}



#endif /* TFC_SERVICEMODEL_CLIENTENDPOINT_H_ */
