/*
 * Serialization.h
 *
 *  Created on: Jan 18, 2017
 *      Author: gilang
 */

#ifndef TFC_SERIALIZATION_H_
#define TFC_SERIALIZATION_H_

#include <tuple>
#include "TFC/Core.h"
#include "TFC/Core/Introspect.h"
#include "TFC/Core/Metaprogramming.h"

namespace TFC {
namespace Serialization {

TFC_ExceptionDeclare	(SerializationException, TFCException);

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

template<typename TDeclaring, typename... TField>
struct TypeSerializationInfo
{
	typedef TDeclaring DeclaringType;
	typedef std::tuple<TField...> FieldList;
};

template<typename TDeclaring>
struct TypeSerializationInfoSelector
{
	static constexpr bool available = false;
};

template<typename TPredicates>
struct PredicateEvaluator;

template<typename TDeclaring,
		 typename TValueType,
		 TValueType TDeclaring::* memPtr,
		 typename TPredicates = std::tuple<>,
		 typename = void>
struct FieldInfo
{
	typedef TValueType ValueType;

	static TValueType const& Get(TDeclaring const& ptr) 			{ return ptr.*memPtr; }
	static void 			 Set(TDeclaring& ptr, TValueType&& val) { ptr.*memPtr = std::move(val); }
	static bool 			 Evaluate(TDeclaring const& ptr) 		{ return PredicateEvaluator<TPredicates>::Evaluate(ptr); }
};

template<typename TSerializerClass, typename TDeclaring, typename = void>
struct GenericSerializer;

template<typename TSerializerClass, typename TDeclaring, typename = void>
struct GenericDeserializer;

template<typename TDeserializerClass, typename... TArgs>
struct ParameterDeserializerFunctor
{
	typedef typename Core::Metaprogramming::SequenceGenerator<sizeof...(TArgs)>::Type ArgSequence;

	template<int... S>
	static std::tuple<typename std::decay<TArgs>::type...> Func(TDeserializerClass& p, Core::Metaprogramming::Sequence<S...>)
	{
		return std::make_tuple(p.template Deserialize<typename std::decay<TArgs>::type>(S)...);
	}

	static std::tuple<typename std::decay<TArgs>::type...> Func(TDeserializerClass& p)
	{
		return Func(p, ArgSequence());
	}
};

}}

#include "TFC/Serialization/SerializerFunctor.h"
#include "TFC/Serialization/PredicateEvaluator.h"

#define TFC_DefineTypeSerializationInfo( CLASS, ... ) \
	template<> \
	struct TFC::Serialization::TypeSerializationInfoSelector< CLASS > \
	{ \
		static constexpr bool available = true; \
		typedef TFC::Serialization::TypeSerializationInfo< CLASS , ##__VA_ARGS__ > Type; \
	}

#define TFC_FieldInfo(MEMPTR, ...) TFC::Serialization::FieldInfo<typename TFC::Core::Introspect::MemberField<decltype( & MEMPTR )>::DeclaringType, decltype( MEMPTR ), & MEMPTR, std::tuple < __VA_ARGS__ > >


#endif /* TFC_SERIALIZATION_H_ */
