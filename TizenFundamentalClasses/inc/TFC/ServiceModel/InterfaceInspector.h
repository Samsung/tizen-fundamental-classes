/*
 * InterfaceInspector.h
 *
 *  Created on: Dec 21, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_
#define TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_


#include <tuple>
#include <typeinfo>
#include <dlog.h>
#include "TFC/Core/Introspect.h"

namespace TFC {
namespace ServiceModel {

template<typename TPackerPolicy, typename... TArgs>
struct PackerFunctor;

template<typename TPackerPolicy, typename TCurrent, typename... TArgs>
struct PackerFunctor<TPackerPolicy, TCurrent, TArgs...>
{
	static void Func(TPackerPolicy& p, TCurrent t, TArgs... next)
	{
		p.Pack(t);
		PackerFunctor<TPackerPolicy, TArgs...>::Func(p, next...);
	}
};

template<typename TPackerPolicy>
struct PackerFunctor<TPackerPolicy>
{
	static void Func(TPackerPolicy& p)
	{

	}
};

template<typename TPackerPolicy, typename... TArgs>
typename TPackerPolicy::PackType PackerFunction(TArgs... args)
{
	TPackerPolicy packer;
	PackerFunctor<TPackerPolicy, TArgs...>::Func(packer, args...);

	return packer.EndPack();
}




template<typename TPackerPolicy,
		 typename TFunctionType,
		 typename TParameterPack = typename TFC::Core::Introspect::MemberFunction<TFunctionType>::ArgsTuple>
struct ParameterPacker
{

};

template<typename TPackerPolicy,
		 typename TFunctionType,
		 typename... TArgs>
struct ParameterPacker<TPackerPolicy, TFunctionType, std::tuple<TArgs...>>
{
	typename TPackerPolicy::PackType operator()(TArgs... param)
	{
		return PackerFunction<TPackerPolicy>(param...);
	}
};

template<typename TPackerPolicy,
		 typename TFunctionType,
		 TFunctionType funcPtr,
		 typename TParameterPack = typename TFC::Core::Introspect::MemberFunction<TFunctionType>::ArgsTuple>
struct ParameterUnpacker;

template<typename TPackerPolicy, typename... TArgs>
struct UnpackerFunctor
{
	static std::tuple<TArgs...> Func(TPackerPolicy& p)
	{
		int index = 0;
		return std::make_tuple(p.template Unpack<TArgs>(index++)...);
	}
};

template<int ...> struct seq {};

template<int N, int ...S> struct gens : gens<N-1, N-1, S...> {};

template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };

template<typename TPackerPolicy,
		 typename TFunctionType,
		 TFunctionType funcPtr,
		 typename... TArgs>
struct ParameterUnpacker<TPackerPolicy, TFunctionType, funcPtr, std::tuple<TArgs...>>
{
	typedef typename TFC::Core::Introspect::MemberFunction<TFunctionType>::DeclaringType InstanceType;
	typedef typename TFC::Core::Introspect::MemberFunction<TFunctionType>::ReturnType ReturnType;


	template<int... S>
	ReturnType Call(InstanceType* i, std::tuple<TArgs...> const& param, seq<S...>)
	{
		return (i->*funcPtr)(std::get<S>(param)...);
	}

	ReturnType operator()(typename TPackerPolicy::PackType p, InstanceType* i)
	{
		TPackerPolicy unpacker(p);
		auto unpacked = UnpackerFunctor<TPackerPolicy, TArgs...>::Func(unpacker);
		return Call(i, unpacked, typename gens<sizeof...(TArgs)>::type());
	}


};

}}



#endif /* TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_ */
