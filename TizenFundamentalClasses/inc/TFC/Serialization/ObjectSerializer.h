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
 *    Serialization/DiscriminatedUnionSerializer.h
 *
 * Wrapper serializer for C++ objects
 *
 * Created on:  Jan 18, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef TFC_SERIALIZATION_OBJECTSERIALIZER_H_
#define TFC_SERIALIZATION_OBJECTSERIALIZER_H_

#include "TFC/Serialization.h"

namespace TFC {
namespace Serialization {

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

template<typename TDeserializerClass, typename TObj>
struct ObjectDeserializer
{
	static TObj Deserialize(typename TDeserializerClass::SerializedType p)
	{
		TDeserializerClass unpacker(p);
		return GenericDeserializer<TDeserializerClass, TObj>::Deserialize(unpacker); //std::get<0>(ParameterDeserializerFunctor<TDeserializerClass, TObj>::Func(unpacker));
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



}}


#endif /* TFC_SERIALIZATION_OBJECTSERIALIZER_H_ */
