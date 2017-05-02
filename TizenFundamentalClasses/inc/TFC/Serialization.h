/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Serialization.h
 *
 * Serialization infrastructure to automatically generate serialization
 * functions for C++ classes. This headers need to be accompanied by
 * other headers within Serialization folder in order to work correctly.
 *
 * Created on: 	 Jan 18, 2017
 * Author: 		 Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */
#pragma once

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

	template<typename TDeserializerClass>
	static void DeserializeAndSet(TDeclaring& ptr, TDeserializerClass& deser) { deser.Deserialize(ptr.*memPtr); }
};

template<typename TDeclaring,
		 typename TValueType,
		 TValueType TDeclaring::* memPtr,
		 typename TPredicates>
struct FieldInfo<TDeclaring, TValueType, memPtr, TPredicates, typename std::enable_if<std::is_enum<TValueType>::value>::type>
{
	typedef TValueType ValueType;

	static TValueType const& Get(TDeclaring const& ptr) 			{ return ptr.*memPtr; }
	static void 			 Set(TDeclaring& ptr, TValueType&& val) { ptr.*memPtr = std::move(val); }
	static bool 			 Evaluate(TDeclaring const& ptr) 		{ return PredicateEvaluator<TPredicates>::Evaluate(ptr); }

	template<typename TDeserializerClass>
	static void DeserializeAndSet(TDeclaring& ptr, TDeserializerClass& deser) { deser.Deserialize(*reinterpret_cast<typename std::underlying_type<TValueType>::type*>(&(ptr.*memPtr))); }
};

template<typename TValueType,
		 TValueType theValue,
		 typename TPredicates>
struct ConstantValue
{
	typedef TValueType ValueType;
	static TValueType theValueStorage;

	template<typename T>
	static TValueType const& Get(T& ptr) 						{ return theValueStorage; }

	template<typename T>
	static void 			 Set(T& ptr, TValueType&& val) 		{  }

	template<typename TDeclaring>
	static bool 			 Evaluate(TDeclaring const& ptr) 		{ return PredicateEvaluator<TPredicates>::Evaluate(ptr); }

	template<typename TDeclaring, typename TDeserializerClass>
	static void DeserializeAndSet(TDeclaring& ptr, TDeserializerClass& deser)
	{
		TValueType val;
		deser.Deserialize(val);
		TFCAssert<SerializationException>(val == theValue, "Unexpected constant value retrieved");
	}
};

template<typename TSerializer, typename TValue>
class SerializationAvailable
{
	typedef char Correct;
	typedef long long Incorrect;

	template<typename T1, typename T2> static Correct Test(decltype(std::declval<T1>().Serialize(std::declval<T2>()))*);
	template<typename T1, typename T2> static Incorrect Test(...);

public:
	static constexpr bool value = sizeof(Test<TSerializer, TValue>(0)) == sizeof(Correct);
};

template<typename TDeserializer, typename TValue>
class DeserializationAvailable
{
	typedef char Correct;
	typedef long long Incorrect;


	template<typename T>
	static typename std::add_lvalue_reference<T>::type declref();


	template<typename T1, typename T2> static Correct Test(decltype(std::declval<T1>().Deserialize(declref<T2>()))*);
	template<typename T1, typename T2> static Incorrect Test(...);

public:
	static constexpr bool value = sizeof(Test<TDeserializer, TValue>(0)) == sizeof(Correct);
};

template<typename TSerializerClass, typename TDeclaring, typename = void>
struct GenericSerializer;

template<typename TSerializerClass, typename TDeclaring>
struct GenericSerializer<TSerializerClass, TDeclaring, typename std::enable_if<SerializationAvailable<TSerializerClass, TDeclaring>::value>::type>
{
	static auto Serialize(TDeclaring const& ptr)
	{
		TSerializerClass packer;
		packer.Serialize(ptr);
		return packer.EndPack();
	}

	static void Serialize(TSerializerClass& packer, TDeclaring const& ref)
	{
		packer.Serialize(ref);
	}
};

template<typename TSerializerClass, typename TDeclaring>
struct GenericSerializer<TSerializerClass, TDeclaring, typename std::enable_if<std::is_enum<TDeclaring>::value>::type>
{
	static void Serialize(TSerializerClass& packer, TDeclaring const& ref)
	{
		packer.Serialize((typename std::underlying_type<TDeclaring>::type)ref);
	}
};

template<typename TSerializerClass, typename TDeclaring, typename = void>
struct GenericDeserializer;

template<typename TDeserializerClass, typename TDeclaring>
struct GenericDeserializer<TDeserializerClass, TDeclaring, typename std::enable_if<DeserializationAvailable<TDeserializerClass, TDeclaring>::value>::type>
{
	static TDeclaring Deserialize(TDeserializerClass& deser)
	{
		TDeclaring ret;
		deser.Deserialize(ret);
		return ret;
	}

	static void Deserialize(TDeserializerClass& deser, TDeclaring& ret)
	{
		deser.Deserialize(ret);
	}
};

template<typename TDeserializerClass, typename TDeclaring>
struct GenericDeserializer<TDeserializerClass, TDeclaring, typename std::enable_if<std::is_enum<TDeclaring>::value>::type>
{
	static TDeclaring Deserialize(TDeserializerClass& deser)
	{
		typename std::underlying_type<TDeclaring>::type ret;
		deser.Deserialize(ret);
		return (TDeclaring)ret;
	}

	static void Deserialize(TDeserializerClass& deser, TDeclaring& ret)
	{
		typename std::underlying_type<TDeclaring>::type tmp;
		deser.Deserialize(tmp);
		ret = (TDeclaring)tmp;
	}
};

template<typename TDeserializerClass, typename... TArgs>
struct ParameterDeserializerFunctor
{
	typedef typename Core::Metaprogramming::SequenceGenerator<sizeof...(TArgs)>::Type ArgSequence;


	template<typename T, typename TVoid = void>
	struct DeserializerSelector
	{
		static void Deserialize(TDeserializerClass& pOut, T& ref)
		{
			auto p = pOut.DeserializeScope();
			GenericDeserializer<TDeserializerClass, T>::Deserialize(p, ref);
			//p.Deserialize(p, ref);
			p.Finalize();
		}
	};

	template<typename T>
	struct DeserializerSelector<T, typename std::enable_if<DeserializationAvailable<TDeserializerClass, T>::value>::type>
	{
		static void Deserialize(TDeserializerClass& p, T& ref)
		{
			p.Deserialize(ref);
		}
	};

	template<typename T>
	struct DeserializerSelector<T, typename std::enable_if<std::is_enum<T>::value>::type>
	{
		static void Deserialize(TDeserializerClass& p, T& ref)
		{
			typename std::underlying_type<T>::type val;
			p.Deserialize(val);
			ref = (T)val;
		}
	};

	template<int... S>
	static std::tuple<typename std::decay<TArgs>::type...> Func(TDeserializerClass& p, Core::Metaprogramming::Sequence<S...>)
	{
		//typedef

		std::tuple<typename std::decay<TArgs>::type...> ret;

		int list[] = {(DeserializerSelector<TArgs>::Deserialize(p, std::get<S>(ret)), 0)...};
		(void)list;

		/*
		int list[] = {(p.Deserialize(std::get<S>(ret)), 0)...};
		(void)list;
		*/

		return ret;
	}

	static std::tuple<typename std::decay<TArgs>::type...> Func(TDeserializerClass& p)
	{
		return Func(p, ArgSequence());
	}
};

}}

template<typename TValueType, TValueType theValue, typename TPredicates>
TValueType TFC::Serialization::ConstantValue<TValueType, theValue, TPredicates>::theValueStorage = theValue;

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

#define TFC_ConstantValue(CONSTANT, ...) TFC::Serialization::ConstantValue< decltype( CONSTANT ), CONSTANT, std::tuple < __VA_ARGS__ > >


#endif /* TFC_SERIALIZATION_H_ */
