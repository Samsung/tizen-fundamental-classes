/*
 * DiscriminatedUnionSerializer.h
 *
 *  Created on: Jan 18, 2017
 *      Author: gilang
 */

#ifndef TFC_SERIALIZATION_DISCRIMINATEDUNIONSERIALIZER_H_
#define TFC_SERIALIZATION_DISCRIMINATEDUNIONSERIALIZER_H_

#include "TFC/Serialization/DiscriminatedUnion.h"
#include "TFC/Serialization/ClassSerializer.h"

namespace TFC {
namespace Serialization {

template<typename TDUType, typename... TCase>
struct DiscriminatedUnionField
{

	TDUType const& v;
	uint32_t discriminator;

	DiscriminatedUnionField(TDUType const& v, uint32_t discriminator) : v(v), discriminator(discriminator) { }

	template<typename TSerializerClass>
	void Serialize(TSerializerClass& ser) const
	{
		ser.Serialize(discriminator);
		DiscriminatedUnionSelector<TDUType, TCase...>::Serialize(ser, v, discriminator);
	}
};

template<typename T, bool = std::is_fundamental<T>::value>
struct InPlaceConstructorHelper
{
	static void Assign(T* ptr, T&& val)
	{
		new (ptr) T(val);
	}
};

template<typename T>
struct InPlaceConstructorHelper<T, true>
{
	static void Assign(T* ptr, T&& val)
	{
		*ptr = val;
	}
};

template<typename TDUType, typename TCurrent, typename... TTail>
struct DiscriminatedUnionSelector<TDUType, TCurrent, TTail...>
{
	template<typename TSerializerClass>
	static void Serialize(TSerializerClass& ser, TDUType const& obj, uint32_t discriminator)
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Try serializing with type %s", typeid(decltype(TCurrent::Get(obj))).name());

		if(TCurrent::Match(discriminator))
			SerializerSelect<TSerializerClass, typename TCurrent::ValueType>::Serialize(ser, TCurrent::Get(obj));
			//ser.Serialize();
		else
			DiscriminatedUnionSelector<TDUType, TTail...>::Serialize(ser, obj, discriminator);
	}

	template<typename TDeserializerClass>
	static void DeserializeAndSet(TDeserializerClass& ser, TDUType& obj, uint32_t discriminator, int curIdx)
	{
		if(TCurrent::Match(discriminator))
		{
			InPlaceConstructorHelper<typename TCurrent::ValueType>::Assign(&(obj.*(TCurrent::targetPointer)), ser.template Deserialize<typename TCurrent::ValueType>(curIdx));
		}
		else
			DiscriminatedUnionSelector<TDUType, TTail...>::DeserializeAndSet(ser, obj, discriminator, curIdx);
	}
};

template<typename TDUType>
struct DiscriminatedUnionSelector<TDUType>
{
	template<typename TSerializerClass>
	static void Serialize(TSerializerClass& ser, TDUType const& obj, uint32_t discriminator)
	{

	}

	template<typename TDeserializerClass>
	static void DeserializeAndSet(TDeserializerClass& ser, TDUType const& obj, uint32_t discriminator, int curIdx)
	{

	}
};

template<typename TSerializerClass, typename TDUType>
struct GenericSerializer<TSerializerClass, TDUType, typename std::enable_if<DiscriminatedUnionTypeInfoSelector<TDUType>::isDiscriminatedUnion>::type>
{
	typedef typename DiscriminatedUnionTypeInfoSelector<TDUType>::Type DUTypeInfo;

	static typename TSerializerClass::SerializedType Serialize(TDUType const& ptr)
	{
		TSerializerClass packer;
		auto& value = DUTypeInfo::Get(ptr);
		value.Serialize(packer);
		return packer.EndPack();
	}

	static void Serialize(TSerializerClass& packer, TDUType const& ptr)
	{
		auto& value = DUTypeInfo::Get(ptr);
		value.Serialize(packer);
	}
};

template<typename TDeserializerClass, typename TDeclaring>
struct GenericDeserializer<TDeserializerClass, TDeclaring, typename std::enable_if<DiscriminatedUnionTypeInfoSelector<TDeclaring>::isDiscriminatedUnion>::type>
{
	typedef typename DiscriminatedUnionTypeInfoSelector<TDeclaring>::Type DUTypeInfo;

	static TDeclaring Deserialize(typename TDeserializerClass::SerializedType p, bool finalizePackedObject = true)
	{
		TDeserializerClass deser(p);
		auto ret = DUTypeInfo::Deserialize(p);
		p.Finalize();
		return ret;
	}

	static TDeclaring Deserialize(TDeserializerClass& unpacker)
	{
		return DUTypeInfo::Deserialize(unpacker);
	}
};


template<typename TSerializerClass, typename TDUType, typename... TCase, typename... TArgs>
struct SerializerFunctor<TSerializerClass, DiscriminatedUnionField<TDUType, TCase...>, TArgs...>
{
	static void Func(TSerializerClass& p, DiscriminatedUnionField<TDUType, TCase...>& t, TArgs... next)
	{
		t.Serialize(p);
		// Call SerializerFunctor recursive by passing the TArgs tails as arguments
		SerializerFunctor<TSerializerClass, TArgs...>::Func(p, next...);
	}
};

template<typename TDeserializerClass, typename TDeclaring, typename TDUType, TDUType TDeclaring::* ptrMem, typename TAny, typename TAny2>
struct ClassDeserializerSelect<TDeserializerClass, TDeclaring, TFC::Serialization::FieldInfo<TDeclaring, TDUType, ptrMem, TAny, TAny2>, Core::Metaprogramming::Void_T<typename DiscriminatedUnionTypeInfoSelector<TDUType>::Type>>
{
	typedef typename DiscriminatedUnionTypeInfoSelector<TDUType>::Type DUTypeInfo;
	typedef TFC::Serialization::FieldInfo<TDeclaring, TDUType, ptrMem, TAny, TAny2> FieldInfo;

	static void DeserializeAndSet(TDeserializerClass& p, TDeclaring& obj, int curIdx = 0)
	{
		if(FieldInfo::Evaluate(obj))
		{
			uint32_t discriminator = p.template Deserialize<uint32_t>(curIdx);
			DUTypeInfo::DeserializeAndSet(ptrMem, obj, discriminator, p, curIdx + 1);
		}
		//TField::Set(obj, p.template Deserialize<typename TField::ValueType>(curIdx));
	}
};

}}



#endif /* TFC_SERIALIZATION_DISCRIMINATEDUNIONSERIALIZER_H_ */
