/*
 * InterfaceInspector.h
 *
 *  Created on: Dec 21, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_
#define TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_


#include <tuple>

#include <dlog.h>

#include "TFC/Core/Introspect.h"
#include "TFC/Core/Metaprogramming.h"

namespace TFC {
namespace ServiceModel {

/**
 * Function template which generates operation to perform serialization based on requested type.
 * The resulting code will perform serialization by calling Pack method of TSerializerClass object.
 * TSerializerClass should have Pack method overloaded with all types specified in TArgs.
 *
 * This template generates sequences of instructions calling Pack method using template
 * recursion for each type in TArgs.
 */
template<typename TSerializerClass, typename... TArgs>
struct SerializerFunctor;

/**
 * Recursion case for SerializerFunctor
 */
template<typename TSerializerClass, typename TCurrent, typename... TArgs>
struct SerializerFunctor<TSerializerClass, TCurrent, TArgs...>
{
	static void Func(TSerializerClass& p, TCurrent t, TArgs... next)
	{
		p.Pack(t);
		// Call SerializerFunctor recursive by passing the TArgs tails as arguments
		SerializerFunctor<TSerializerClass, TArgs...>::Func(p, next...);
	}
};

/**
 * Base case for SerializerFunctor where TArgs is nil entirely
 */
template<typename TSerializerClass>
struct SerializerFunctor<TSerializerClass>
{
	// Base function just do nothing
	static void Func(TSerializerClass& p) { }
};

/**
 * ParameterSerializer templates generates the serialization operations based on TFunctionType
 * pointer-to-member-function. This template extracts the parameter list on TFunctionType using
 * introspector template (Core::Introspect::MemberFunction<>).
 *
 * This template declaration basically only specify the class template structure with its default
 * argument. The actual definition will extract the pointer-to-member parameter list from static
 * list retrieved from introspector template.
 */
template<typename TSerializerClass,
		 typename TFunctionType,
		 typename TParameterPack = typename Core::Introspect::MemberFunction<TFunctionType>::ArgsTuple>
struct ParameterSerializer;

/**
 * This is the definition (and specialization) for ParameterSerializer template. The introspector class
 * specified in its declaration supply the TParameterPack (third argument of the template) with the
 * static-list of the pointer-to-member-function's parameter types.
 *
 * The TParameterPack which is a tuple with list of arguments is extracted using variadic template,
 * then itself is supplied to the SerializerFunctor to generates the serialization instructions.
 */
template<typename TSerializerClass,
		 typename TFunctionType,
		 typename... TArgs>
struct ParameterSerializer<TSerializerClass, TFunctionType, std::tuple<TArgs...>>
{
	static typename TSerializerClass::PackType Serialize(TArgs const&... param)
	{
		TSerializerClass packer;
		SerializerFunctor<TSerializerClass, TArgs...>::Func(packer, param...);
		return packer.EndPack();
	}
};

template<typename TSerializerClass,
		 typename TObj>
struct ObjectSerializer
{
	static typename TSerializerClass::PackType Serialize(TObj const& obj)
	{
		TSerializerClass packer;
		SerializerFunctor<TSerializerClass, TObj>::Func(packer, obj);
		return packer.EndPack();
	}
};

template<typename TSerializerClass>
struct ObjectSerializer<TSerializerClass, void>
{
	static typename TSerializerClass::PackType Serialize()
	{
		TSerializerClass packer;
		return packer.EndPack();
	}
};



template<typename TDeserializerClass,
		 typename TFunctionType,
		 typename TParameterPack = typename Core::Introspect::MemberFunction<TFunctionType>::ArgsTuple>
struct ParameterDeserializer;

template<typename TDeserializerClass, typename... TArgs>
struct ParameterDeserializerFunctor
{
	typedef typename Core::Metaprogramming::SequenceGenerator<sizeof...(TArgs)>::Type ArgSequence;

	template<int... S>
	static std::tuple<TArgs...> Func(TDeserializerClass& p, Core::Metaprogramming::Sequence<S...>)
	{
		return std::make_tuple(p.template Unpack<TArgs>(S)...);
	}

	static std::tuple<TArgs...> Func(TDeserializerClass& p)
	{
		return Func(p, ArgSequence());
	}
};

template<typename TDeserializerClass,
		 typename TFunctionType,
		 typename... TArgs>
struct ParameterDeserializer<TDeserializerClass, TFunctionType, std::tuple<TArgs...>>
{
	static std::tuple<TArgs...> Deserialize(typename TDeserializerClass::PackType p, bool finalizePackedObject = true)
	{
		TDeserializerClass unpacker(p);
		return ParameterDeserializerFunctor<TDeserializerClass, TArgs...>::Func(unpacker);

		if(finalizePackedObject)
			unpacker.Finalize();
	}
};

template<typename TDeserializerClass, typename TObj>
struct ObjectDeserializer
{
	static TObj Deserialize(typename TDeserializerClass::PackType p)
	{
		TDeserializerClass unpacker(p);
		return std::get<0>(ParameterDeserializerFunctor<TDeserializerClass, TObj>::Func(unpacker));
	}
};

template<typename TDeserializerClass>
struct ObjectDeserializer<TDeserializerClass, void>
{
	static void Deserialize(typename TDeserializerClass::PackType p)
	{
		return;
	}
};

template<typename TFunctionType,
		 typename TParameterPack = typename Core::Introspect::MemberFunction<TFunctionType>::ArgsTuple>
struct DelayedInvoker;

template<typename TFunctionType, typename... TArgs>
struct DelayedInvoker<TFunctionType, std::tuple<TArgs...>>
{
	typedef typename Core::Introspect::MemberFunction<TFunctionType>::DeclaringType InstanceType;
	typedef typename Core::Introspect::MemberFunction<TFunctionType>::ReturnType ReturnType;
	typedef typename Core::Metaprogramming::SequenceGenerator<sizeof...(TArgs)>::Type ArgSequence;

	template<int... S>
	static ReturnType Call(InstanceType* i, TFunctionType ptr, std::tuple<TArgs...> const& param, Core::Metaprogramming::Sequence<S...>)
	{
		return (i->*ptr)(std::get<S>(param)...);
	}

	static ReturnType Invoke(InstanceType* i, TFunctionType ptr, std::tuple<TArgs...> const& args)
	{
		return Call(i, ptr, args, ArgSequence());
	}
};

}}



#endif /* TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_ */
