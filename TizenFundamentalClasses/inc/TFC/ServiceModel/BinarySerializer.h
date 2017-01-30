/*
 * KeyValuePairSerializer.h
 *
 *  Created on: Jan 30, 2017
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_BINARYSERIALIZER_H_
#define TFC_SERVICEMODEL_BINARYSERIALIZER_H_

#include <string>
#include <vector>
#include <sstream>

#include "TFC/Core/Introspect.h"
#include "TFC/Core/Reflection.h"
#include "TFC/ServiceModel/Includes.h"
#include "TFC/ServiceModel/ServerEndpoint.h"
#include "TFC/Serialization/ClassSerializer.h"
#include "TFC/Serialization/DiscriminatedUnionSerializer.h"

namespace TFC {
namespace ServiceModel {

struct BinarySerializer
{
	typedef std::vector<uint8_t> SerializedType;

	SerializedType buffer;

	BinarySerializer();

	void Serialize(uint32_t args);
	void Serialize(int64_t args);
	void Serialize(int args);
	void Serialize(bool args);
	void Serialize(double args);
	void Serialize(std::string const& args);

	void Serialize(SerializedType const& p);

	SerializedType EndPack();
};

struct BinaryDeserializer
{
	typedef std::vector<uint8_t> SerializedType;

private:
	SerializedType const& bufferRef;
	size_t currentPos;



	template<typename T>
	T DeserializeImpl();

public:
	BinaryDeserializer(SerializedType const& p);

	template<typename, typename> friend struct DeserializeSelector;

	template<typename T, typename = void>
	struct DeserializerSelector
	{
		static T Deserialize(BinaryDeserializer& deser)
		{
			return deser.DeserializeImpl<T>();
		}
	};

	template<typename T>
	T Deserialize(int index)
	{
		return DeserializerSelector<T>::Deserialize(*this);
	}

	void Finalize();
};

template<typename T>
struct BinaryDeserializer::DeserializerSelector<T, Core::Metaprogramming::Void_T<typename Serialization::TypeSerializationInfoSelector<T>::Type>>
{
	static T Deserialize(BinaryDeserializer& deser)
	{
		using namespace TFC::Serialization;
		return ClassDeserializer<BinaryDeserializer, T>::Deserialize(deser);
	}
};

template<typename T>
struct BinaryDeserializer::DeserializerSelector<T, Core::Metaprogramming::Void_T<typename std::enable_if<std::is_enum<T>::value>::type>>
{
	static T Deserialize(BinaryDeserializer& deser)
	{

	}
};

template<typename T>
struct BinaryDeserializer::DeserializerSelector<T, Core::Metaprogramming::Void_T<typename Serialization::DiscriminatedUnionTypeInfoSelector<T>::Type>>
{
	static T Deserialize(BinaryDeserializer& deser)
	{
		using namespace TFC::Serialization;
		return GenericDeserializer<BinaryDeserializer, T>::Deserialize(deser);

	}
};

template<typename T>
struct BinaryDeserializer::DeserializerSelector<std::vector<T>, Core::Metaprogramming::Void_T<typename Serialization::TypeSerializationInfoSelector<T>::Type>>
{
	static std::vector<T> Deserialize(BinaryDeserializer& deser)
	{
		using namespace TFC::Serialization;

	}
};

}
}



#endif /* TFC_SERVICEMODEL_BINARYSERIALIZER_H_ */
