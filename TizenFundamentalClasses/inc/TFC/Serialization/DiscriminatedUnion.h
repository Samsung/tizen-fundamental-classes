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
 *    Serialization/DiscriminatedUnion.h
 *
 * Classes to declare serialization process for C++ unions
 *
 * Created on:  Jan 18, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#ifndef TFC_SERIALIZATION_DISCRIMINATEDUNION_H_
#define TFC_SERIALIZATION_DISCRIMINATEDUNION_H_

#include "TFC/Serialization.h"

namespace TFC {
namespace Serialization {

template<typename TDUType>
struct DiscriminatedUnionTypeInfoSelector
{
	static constexpr bool isDiscriminatedUnion = false;
};

template<typename TDUType, typename... TTail>
struct DiscriminatedUnionSelector;

template<typename TDUType, typename... TCase>
struct DiscriminatedUnionField;

template<typename TDUType, typename TDiscriminator, TDiscriminator TDUType::* discriminator, typename... TCase>
struct DiscriminatedUnionTypeInfo
{
	typedef DiscriminatedUnionField<TDUType, TCase...> ValueType;

	template<typename TDeclaring>
	static ValueType const Get(TDUType TDeclaring::* memPtr, TDeclaring const& ptr)
	{
		auto& val =  ptr.*memPtr;
		uint32_t disc = (uint32_t)(val.*discriminator);
		return { val, disc };
	}

	static ValueType const Get(TDUType const& obj)
	{
		uint32_t disc = uint32_t(obj.*discriminator);
		return { obj, disc };
	}

	template<typename TDeclaring, typename TDeserializerClass>
	static void DeserializeAndSet(TDUType TDeclaring::* memPtr, TDeclaring& ptr, uint32_t discriminatorVal, TDeserializerClass& deser, int curIdx)
	{
		auto& val = ptr.*memPtr;
		val.*discriminator = (TDiscriminator)discriminatorVal;
		DiscriminatedUnionSelector<TDUType, TCase...>::DeserializeAndSet(deser, val, discriminatorVal, curIdx);
	}

	template<typename TDeserializerClass>
	static void DeserializeAndSet(TDUType& ref, TDeserializerClass& deser)
	{
		uint32_t discriminatorVal = 0;
		deser.Deserialize(discriminatorVal);

		ref.*discriminator = (TDiscriminator)discriminatorVal;
		DiscriminatedUnionSelector<TDUType, TCase...>::DeserializeAndSet(deser, ref, discriminatorVal, 0);
	}

	template<typename TDeserializerClass>
	static TDUType Deserialize(TDeserializerClass& deser)
	{
		TDUType ret;
		uint32_t discriminatorVal = 0;
		deser.Deserialize(discriminatorVal);

		ret.*discriminator = (TDiscriminator)discriminatorVal;
		DiscriminatedUnionSelector<TDUType, TCase...>::DeserializeAndSet(deser, ret, discriminatorVal, 0);
		return ret;
	}

	template<typename TDeserializerClass>
	static TDUType Deserialize(TDeserializerClass& deser, uint32_t discriminatorVal)
	{
		TDUType ret;
		ret.*discriminator = (TDiscriminator)discriminatorVal;
		DiscriminatedUnionSelector<TDUType, TCase...>::DeserializeAndSet(deser, ret, discriminatorVal, 0);
		return ret;
	}
};

template<typename TDeclaring, typename TValueType, TValueType TDeclaring::* memPtr, typename TPredicates>
struct FieldInfo<TDeclaring, TValueType, memPtr, TPredicates, Core::Metaprogramming::Void_T<typename DiscriminatedUnionTypeInfoSelector<TValueType>::Type>>
{
	typedef typename DiscriminatedUnionTypeInfoSelector<TValueType>::Type DUTypeInfo;

	typedef typename DUTypeInfo::ValueType const ValueType;

	static ValueType Get(TDeclaring const& ptr)
	{
		return DUTypeInfo::Get(memPtr, ptr);
	}

	static bool Evaluate(TDeclaring const& ptr)
	{
		return PredicateEvaluator<TPredicates>::Evaluate(ptr);
	}
};

template<uint32_t discriminatorV, typename TDUType, typename TValType, TValType TDUType::* targetPtr>
struct DiscriminatedUnionCase
{
	static uint32_t const discriminator = discriminatorV;
	typedef TValType ValueType;
	static constexpr auto targetPointer = targetPtr;

	static constexpr bool Match(uint32_t v) { return discriminator == v; }
	static ValueType const& Get(TDUType const& o) { return o.*targetPtr; }
};



}}

#define TFC_DefineDiscriminatedUnionType(TYPENAME, DISCR, ... ) \
	template<> \
	struct TFC::Serialization::DiscriminatedUnionTypeInfoSelector < TYPENAME > \
	{ \
		static constexpr bool isDiscriminatedUnion = true; \
		typedef TFC::Serialization::DiscriminatedUnionTypeInfo< TYPENAME , decltype( TYPENAME :: DISCR ), & TYPENAME :: DISCR , __VA_ARGS__ > Type; \
	}

#define TFC_UnionCase(DISCR, MEMPTR ) TFC::Serialization::DiscriminatedUnionCase< (uint32_t) DISCR, typename TFC::Core::Introspect::MemberField< decltype( & MEMPTR ) >::DeclaringType, decltype( MEMPTR ), & MEMPTR>


#endif /* TFC_SERIALIZATION_DISCRIMINATEDUNION_H_ */
