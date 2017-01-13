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



std::string GetInterfaceName(char const* prefix, std::type_info const& i);


/**
 * Function template which generates operation to perform serialization based on requested type.
 * The resulting code will perform serialization by calling Serialize method of TSerializerClass object.
 * TSerializerClass should have Serialize method overloaded with all types specified in TArgs.
 *
 * This template generates sequences of instructions calling Serialize method using template
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
		p.Serialize(t);
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
	static typename TSerializerClass::SerializedType Serialize(TArgs const&... param)
	{
		TSerializerClass packer;
		SerializerFunctor<TSerializerClass, TArgs...>::Func(packer, param...);
		return packer.EndPack();
	}

	static void Serialize(typename TSerializerClass::SerializedType& packer, TArgs const&... param)
	{
		SerializerFunctor<TSerializerClass, TArgs...>::Func(packer, param...);
	}
};

template<typename TDeclaring, typename TValueType, TValueType TDeclaring::* memPtr>
struct FieldInfo
{
	typedef TValueType ValueType;

	static TValueType const& Get(TDeclaring const& ptr)
	{
		return ptr.*memPtr;
	}

	static void Set(TDeclaring& ptr, TValueType&& val)
	{
		ptr.*memPtr = std::move(val);
	}
};

template<typename TDeclaring, typename... TField>
struct TypeSerializationInfo
{
	typedef TDeclaring DeclaringType;
	typedef std::tuple<TField...> FieldList;
};

template<typename TDeclaring>
struct TypeSerializationInfoSelector;

template<typename TSerializerClass, typename TDeclaring, typename TSerializationInfo = typename TypeSerializationInfoSelector<TDeclaring>::Type>
struct ClassSerializer;

template<typename TSerializerClass, typename TDeclaring, typename... TField>
struct ClassSerializer<TSerializerClass, TDeclaring, TypeSerializationInfo<TDeclaring, TField...>>
{

	static typename TSerializerClass::SerializedType Serialize(TDeclaring const& ptr)
	{
		TSerializerClass packer;
		SerializerFunctor<TSerializerClass, typename TField::ValueType...>::Func(packer, TField::Get(ptr)...);
		return packer.EndPack();
	}

	static void Serialize(TSerializerClass& packer, TDeclaring const& ptr)
	{
		SerializerFunctor<TSerializerClass, typename TField::ValueType...>::Func(packer, TField::Get(ptr)...);
	}
};

template<typename TSerializerClass, typename TFieldList>
struct TupleSerializer;

template<typename TSerializerClass, typename... TField>
struct TupleSerializer<TSerializerClass, std::tuple<TField...>>
{
	typedef typename Core::Metaprogramming::SequenceGenerator<sizeof...(TField)>::Type ArgSequence;

	template<int... S>
	static typename TSerializerClass::SerializedType Serialize(std::tuple<TField...> const& ptr, Core::Metaprogramming::Sequence<S...>)
	{
		TSerializerClass packer;
		SerializerFunctor<TSerializerClass, TField...>::Func(packer, std::get<S>(ptr)...);
		return packer.EndPack();
	}

	static typename TSerializerClass::SerializedType Serialize(std::tuple<TField...> const& ptr)
	{
		return Serialize(ptr, ArgSequence());
	}

	template<int... S>
	static void Serialize(TSerializerClass& packer, std::tuple<TField...> const& ptr, Core::Metaprogramming::Sequence<S...>)
	{
		SerializerFunctor<TSerializerClass, TField...>::Func(packer, std::get<S>(ptr)...);
	}

	static void Serialize(TSerializerClass& packer, std::tuple<TField...> const& ptr)
	{
		Serialize(packer, ptr, ArgSequence());
	}
};

template<typename TSerializerClass, typename TDeclaring>
struct ClassSerializerSelector;

template<typename TSerializerClass,
		 typename TObj>
struct ObjectSerializer
{
	static typename TSerializerClass::SerializedType Serialize(TObj const& obj)
	{
		TSerializerClass packer;
		SerializerFunctor<TSerializerClass, TObj>::Func(packer, obj);
		return packer.EndPack();
	}
};

template<typename TSerializerClass>
struct ObjectSerializer<TSerializerClass, void>
{
	static typename TSerializerClass::SerializedType Serialize()
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
		return std::make_tuple(p.template Deserialize<TArgs>(S)...);
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
	static std::tuple<TArgs...> Deserialize(typename TDeserializerClass::SerializedType p, bool finalizePackedObject = true)
	{
		TDeserializerClass unpacker(p);

		if(finalizePackedObject)
			unpacker.Finalize();

		return ParameterDeserializerFunctor<TDeserializerClass, TArgs...>::Func(unpacker);
	}
};

template<typename TDeserializerClass, typename TDeclaring, typename... TFieldArgs>
struct ClassDeserializerFunctor;

template<typename TDeserializerClass, typename TDeclaring, typename TCurrentField, typename... TRemainArgs>
struct ClassDeserializerFunctor<TDeserializerClass, TDeclaring, TCurrentField, TRemainArgs...>
{
	static void Func(TDeserializerClass& p, TDeclaring& obj, int curIdx = 0)
	{
		TCurrentField::Set(obj, p.template Deserialize<typename TCurrentField::ValueType>(curIdx));
		ClassDeserializerFunctor<TDeserializerClass, TDeclaring, TRemainArgs...>::Func(p, obj, curIdx + 1);
	}
};

template<typename TDeserializerClass, typename TDeclaring>
struct ClassDeserializerFunctor<TDeserializerClass, TDeclaring>
{
	static void Func(TDeserializerClass& p, TDeclaring& obj, int curIdx = 0)
	{

	}
};


template<typename TDeserializerClass, typename TDeclaring, typename TSerializationInfo = typename TypeSerializationInfoSelector<TDeclaring>::Type>
struct ClassDeserializer;

template<typename TDeserializerClass, typename TDeclaring, typename... TFieldArgs>
struct ClassDeserializer<TDeserializerClass, TDeclaring, TypeSerializationInfo<TDeclaring, TFieldArgs...>>
{

	static TDeclaring Deserialize(typename TDeserializerClass::SerializedType p, bool finalizePackedObject = true)
	{
		TDeserializerClass unpacker(p);

		TDeclaring ret;
		ClassDeserializerFunctor<TDeserializerClass, TDeclaring, TFieldArgs...>::Func(unpacker, ret);

		if(finalizePackedObject)
			unpacker.Finalize();

		return ret;
	}

	static TDeclaring Deserialize(TDeserializerClass& unpacker)
	{
		TDeclaring ret;
		ClassDeserializerFunctor<TDeserializerClass, TDeclaring, TFieldArgs...>::Func(unpacker, ret);
		return ret;
	}
};

template<typename TDeserializerClass, typename TObj>
struct ObjectDeserializer
{
	static TObj Deserialize(typename TDeserializerClass::SerializedType p)
	{
		TDeserializerClass unpacker(p);
		return std::get<0>(ParameterDeserializerFunctor<TDeserializerClass, TObj>::Func(unpacker));
	}
};

template<typename TDeserializerClass>
struct ObjectDeserializer<TDeserializerClass, void>
{
	static void Deserialize(typename TDeserializerClass::SerializedType p)
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

#define TFC_FieldInfo(MEMPTR) TFC::ServiceModel::FieldInfo<typename TFC::Core::Introspect::MemberField<decltype( & MEMPTR )>::DeclaringType, decltype( MEMPTR ), & MEMPTR>


#define TFC_DefineTypeSerializationInfo( CLASS, ... ) \
	template<> \
	struct TFC::ServiceModel::TypeSerializationInfoSelector< CLASS > \
	{ \
		typedef TFC::ServiceModel::TypeSerializationInfo< CLASS , __VA_ARGS__ > Type; \
	}

#endif /* TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_ */
