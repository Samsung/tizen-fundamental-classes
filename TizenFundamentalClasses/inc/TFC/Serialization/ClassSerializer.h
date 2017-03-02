/*
 * ClassSerializer.h
 *
 *  Created on: Jan 18, 2017
 *      Author: gilang
 */

#ifndef TFC_SERIALIZATION_CLASSSERIALIZER_H_
#define TFC_SERIALIZATION_CLASSSERIALIZER_H_

#include "TFC/Serialization.h"
#include "TFC/Core/Metaprogramming.h"

namespace TFC {
namespace Serialization {

template<typename TSerializerClass, typename TDeclaring, typename TSerializationInfo = typename TypeSerializationInfoSelector<TDeclaring>::Type>
struct ClassSerializer;

template<typename TDeserializerClass, typename TDeclaring, typename TSerializationInfo = typename TypeSerializationInfoSelector<TDeclaring>::Type>
struct ClassDeserializer;

template<typename TSerializerClass, typename TDeclaring>
struct GenericSerializer<TSerializerClass, TDeclaring, typename std::enable_if<TypeSerializationInfoSelector<TDeclaring>::available>::type>
{
	typedef ClassSerializer<TSerializerClass, TDeclaring> Serializer;

	static auto Serialize(TDeclaring const& ptr)
		-> decltype(Serializer::Serialize(ptr))
	{
		return Serializer::Serialize(ptr);
	}

	static void Serialize(TSerializerClass& packer, TDeclaring const& ptr)
	{
		Serializer::Serialize(packer, ptr);
	}
};



template<typename TDeserializerClass, typename TDeclaring>
struct GenericDeserializer<TDeserializerClass, TDeclaring, typename std::enable_if<TypeSerializationInfoSelector<TDeclaring>::available>::type>
{
	typedef ClassDeserializer<TDeserializerClass, TDeclaring> Deserializer;

	static auto Deserialize(typename TDeserializerClass::SerializedType p, bool finalizePackedObject = true)
		-> decltype(Deserializer::Deserialize(p, finalizePackedObject))
	{
		return Deserializer::Deserialize(p, finalizePackedObject);
	}

	static TDeclaring Deserialize(TDeserializerClass& unpacker)
	{
		return Deserializer::Deserialize(unpacker);
	}

	static void Deserialize(TDeserializerClass& unpacker, TDeclaring& ret)
	{
		Deserializer::Deserialize(unpacker, ret);
	}
};

template<typename TSerializerClass, typename TDeclaring, typename... TField>
struct ClassSerializer<TSerializerClass, TDeclaring, TypeSerializationInfo<TDeclaring, TField...>>
{
	static typename TSerializerClass::SerializedType Serialize(TDeclaring const& ptr)
	{
		TSerializerClass packer;
		SerializerFunctor<TSerializerClass, typename TField::ValueType...>::Func(packer, SerializerField<typename TField::ValueType> { TField::Get(ptr), TField::Evaluate(ptr) }...);
		return packer.EndPack();
	}

	static void Serialize(TSerializerClass& packer, TDeclaring const& ptr)
	{
		SerializerFunctor<TSerializerClass, typename TField::ValueType...>::Func(packer, SerializerField<typename TField::ValueType> { TField::Get(ptr), TField::Evaluate(ptr) }...);
	}
};

template<typename TDeserializerClass, typename TDeclaring, typename TField, typename = void>
struct ClassDeserializerSelect
{
	static void DeserializeAndSet(TDeserializerClass& p, TDeclaring& obj, int curIdx = 0)
	{
		if(TField::Evaluate(obj))
		{
			TField::DeserializeAndSet(obj, p);//.template Deserialize<typename TField::ValueType>(curIdx));
		}
	}
};

template<typename TDeserializerClass, typename TDeclaring, typename... TFieldArgs>
struct ClassDeserializerFunctor;

template<typename TDeserializerClass, typename TDeclaring, typename TCurrentField, typename... TRemainArgs>
struct ClassDeserializerFunctor<TDeserializerClass, TDeclaring, TCurrentField, TRemainArgs...>
{
	static void Func(TDeserializerClass& p, TDeclaring& obj, int curIdx = 0)
	{
		ClassDeserializerSelect<TDeserializerClass, TDeclaring, TCurrentField>::DeserializeAndSet(p, obj, curIdx);
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

	static void Deserialize(TDeserializerClass& unpacker, TDeclaring& ret)
	{
		ClassDeserializerFunctor<TDeserializerClass, TDeclaring, TFieldArgs...>::Func(unpacker, ret);
	}
};

template<typename TDeclaring, typename TValueType, TValueType TDeclaring::* memPtr, typename TPredicates>
struct FieldInfo<TDeclaring, TValueType, memPtr, TPredicates, Core::Metaprogramming::Void_T<typename TypeSerializationInfoSelector<TValueType>::Type>>
{
	typedef typename TypeSerializationInfoSelector<TValueType>::Type TypeInfo;

	typedef TValueType ValueType;

	static TValueType const& Get(TDeclaring const& ptr) 			{ return ptr.*memPtr; }
	static void 			 Set(TDeclaring& ptr, TValueType&& val) { ptr.*memPtr = std::move(val); }
	static bool 			 Evaluate(TDeclaring const& ptr) 		{ return PredicateEvaluator<TPredicates>::Evaluate(ptr); }

	template<typename TDeserializerClass>
	static void DeserializeAndSet(TDeclaring& ptr, TDeserializerClass& deser)
	{
		decltype(auto) deserInner = deser.DeserializeScope();
		GenericDeserializer<TDeserializerClass, TValueType>::Deserialize(deserInner, ptr.*memPtr);
	}
};


}}



#endif /* TFC_SERIALIZATION_CLASSSERIALIZER_H_ */
