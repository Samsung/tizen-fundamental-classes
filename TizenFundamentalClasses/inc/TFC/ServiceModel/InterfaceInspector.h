/*
 * InterfaceInspector.h
 *
 *  Created on: Dec 21, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_
#define TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_


#include <tuple>
#include "TFC/Core/Introspect.h"

namespace TFC {
namespace ServiceModel {

template<typename TSerializerClass, typename... TArgs>
struct SerializerFunctor;

template<typename TSerializerClass, typename TCurrent, typename... TArgs>
struct SerializerFunctor<TSerializerClass, TCurrent, TArgs...>
{
	static void Func(TSerializerClass& p, TCurrent t, TArgs... next)
	{
		p.Pack(t);
		SerializerFunctor<TSerializerClass, TArgs...>::Func(p, next...);
	}
};

template<typename TSerializerClass>
struct SerializerFunctor<TSerializerClass>
{
	static void Func(TSerializerClass& p) { }
};

template<typename TSerializerClass,
		 typename TFunctionType,
		 typename TParameterPack = typename TFC::Core::Introspect::MemberFunction<TFunctionType>::ArgsTuple>
struct ParameterSerializer;

template<typename TSerializerClass,
		 typename TFunctionType,
		 typename... TArgs>
struct ParameterSerializer<TSerializerClass, TFunctionType, std::tuple<TArgs...>>
{
	static typename TSerializerClass::PackType Serialize(TArgs... param)
	{
		TSerializerClass packer;
		SerializerFunctor<TSerializerClass, TArgs...>::Func(packer, param...);
		return packer.EndPack();
	}
};

template<typename TDeserializerClass,
		 typename TFunctionType,
		 typename TParameterPack = typename TFC::Core::Introspect::MemberFunction<TFunctionType>::ArgsTuple>
struct ParameterDeserializer;

template<typename TDeserializerClass, typename... TArgs>
struct ParameterDeserializerFunctor
{
	static std::tuple<TArgs...> Func(TDeserializerClass& p)
	{
		int index = 0;
		return std::make_tuple(p.template Unpack<TArgs>(index++)...);
	}
};

template<int...> struct seq {};

template<int N, int... S> struct gens : gens<N-1, N-1, S...> {};

template<int... S> struct gens<0, S...>{ typedef seq<S...> type; };

template<typename TDeserializerClass,
		 typename TFunctionType,
		 typename... TArgs>
struct ParameterDeserializer<TDeserializerClass, TFunctionType, std::tuple<TArgs...>>
{
	static std::tuple<TArgs...> Deserialize(typename TDeserializerClass::PackType p)
	{
		TDeserializerClass unpacker(p);
		return ParameterDeserializerFunctor<TDeserializerClass, TArgs...>::Func(unpacker);
	}
};

template<typename TFunctionType,
		 typename TParameterPack = typename TFC::Core::Introspect::MemberFunction<TFunctionType>::ArgsTuple>
struct DelayedInvoker;

template<typename TFunctionType, typename... TArgs>
struct DelayedInvoker<TFunctionType, std::tuple<TArgs...>>
{
	typedef typename TFC::Core::Introspect::MemberFunction<TFunctionType>::DeclaringType InstanceType;
	typedef typename TFC::Core::Introspect::MemberFunction<TFunctionType>::ReturnType ReturnType;

	InstanceType* i;
	TFunctionType ptr;


	DelayedInvoker(InstanceType* i, TFunctionType ptr) : i(i), ptr(ptr) { }

	template<int... S>
	ReturnType Call(std::tuple<TArgs...> const& param, seq<S...>)
	{
		return (i->*ptr)(std::get<S>(param)...);
	}

	ReturnType operator()(std::tuple<TArgs...> const& args)
	{
		return Call(args, typename gens<sizeof...(TArgs)>::type());
	}
};

}}



#endif /* TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_ */
