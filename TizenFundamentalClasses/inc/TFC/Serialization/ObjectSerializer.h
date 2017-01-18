/*
 * ObjectSerializer.h
 *
 *  Created on: Jan 18, 2017
 *      Author: gilang
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



}}


#endif /* TFC_SERIALIZATION_OBJECTSERIALIZER_H_ */
