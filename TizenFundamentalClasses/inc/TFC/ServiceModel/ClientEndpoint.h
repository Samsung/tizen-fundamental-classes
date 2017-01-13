/*
 * ClientEndpoint.h
 *
 *  Created on: Dec 27, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_CLIENTENDPOINT_H_
#define TFC_SERVICEMODEL_CLIENTENDPOINT_H_

#include "TFC/Core/Introspect.h"
#include "TFC/ServiceModel/InterfaceInspector.h"
#include "TFC/ServiceModel/Reflection.h"
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

template<typename TEndpoint, typename T>
class ClientEndpoint : public T
{
private:
	std::string objectPath;
	typedef typename TEndpoint::Channel Channel;
	typename Channel::Client endpoint;

	template<typename TMemPtr, typename... TArgs>
	auto InvokeInternal(TMemPtr ptr, TArgs... args)
		-> typename TFC::Core::Introspect::MemberFunction<TMemPtr>::ReturnType
	{
		typedef ParameterSerializer<typename Channel::Serializer, TMemPtr> Serializer;
		typedef ObjectDeserializer<typename Channel::Deserializer, typename TFC::Core::Introspect::MemberFunction<TMemPtr>::ReturnType> ReturnTypeDeserializer;

		auto& typeDescription = TypeInfo<T>::typeDescription;

		auto serialized = Serializer::Serialize(args...);
		dlog_print(DLOG_DEBUG, "RPC-Test", "In InvokeInternal");
		return ReturnTypeDeserializer::Deserialize(endpoint.RemoteCall(typeDescription.GetFunctionNameByPointer(ptr), serialized));
	}
protected:
	template<typename TMemPtr, typename... TArgs>
	auto Invoke(TMemPtr ptr, TArgs... param)
		-> typename TFC::Core::Introspect::MemberFunction<TMemPtr>::ReturnType
	{
		return InvokeInternal<TMemPtr>(ptr, param...);
	}

	ClientEndpoint(char const* objectPath) :
		endpoint(TEndpoint::configuration, objectPath, GetInterfaceName(InterfacePrefixInspector<TEndpoint>::value, typeid(T)).c_str())
	{

	}
public:

};



}}



#endif /* TFC_SERVICEMODEL_CLIENTENDPOINT_H_ */
