/*
 * TupleSerializer.h
 *
 *  Created on: Jan 18, 2017
 *      Author: gilang
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
