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
 *    Serialization/ParameterSerializer.h
 *
 * Wrapper serializer for function parameter
 *
 * Created on:  Jan 18, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef TFC_SERIALIZATION_PARAMETERSERIALIZER_H_
#define TFC_SERIALIZATION_PARAMETERSERIALIZER_H_

#include "TFC/Serialization.h"

namespace TFC {
namespace Serialization {

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
	static typename TSerializerClass::SerializedType Serialize(TArgs... param)
	{
		TSerializerClass packer;
		SerializerFunctor<TSerializerClass, TArgs...>::Func(packer, param...);
		return packer.EndPack();
	}

	static void Serialize(typename TSerializerClass::SerializedType& packer, TArgs... param)
	{
		SerializerFunctor<TSerializerClass, TArgs...>::Func(packer, param...);
	}
};

template<typename TDeserializerClass,
		 typename TFunctionType,
		 typename TParameterPack = typename Core::Introspect::MemberFunction<TFunctionType>::ArgsTupleDecay>
struct ParameterDeserializer;



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

}}



#endif /* TFC_SERIALIZATION_SERIALIZERFUNCTOR_H_ */

