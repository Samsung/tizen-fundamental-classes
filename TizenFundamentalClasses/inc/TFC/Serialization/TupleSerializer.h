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
 *    Serialization/TupleSerializer.h
 *
 * Wrapper for serializing tuple object
 *
 * Created on:  Jan 18, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */


#ifndef TFC_SERIALIZATION_TUPLESERIALIZER_H_
#define TFC_SERIALIZATION_TUPLESERIALIZER_H_

#include "TFC/Serialization.h"

namespace TFC {
namespace Serialization {

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

}}



#endif /* TFC_SERIALIZATION_TUPLESERIALIZER_H_ */
