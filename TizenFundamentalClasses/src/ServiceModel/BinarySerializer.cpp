/*
 * BinarySerializer.cpp
 *
 *  Created on: Jan 30, 2017
 *      Author: gilang
 */

#include "TFC/ServiceModel/BinarySerializer.h"

using namespace TFC::ServiceModel;

LIBAPI
BinarySerializer::BinarySerializer() {
}

LIBAPI
void BinarySerializer::Serialize(uint32_t args)
{
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer.push_back(ref[i]);
}

LIBAPI
void BinarySerializer::Serialize(int64_t args)
{
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer.push_back(ref[i]);

}

LIBAPI
void BinarySerializer::Serialize(int args)
{
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer.push_back(ref[i]);
}

LIBAPI
void BinarySerializer::Serialize(bool args)
{
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer.push_back(ref[i]);
}

LIBAPI
void BinarySerializer::Serialize(double args)
{
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer.push_back(ref[i]);
}

LIBAPI
void BinarySerializer::Serialize(std::string const& args)
{
	auto len = args.length();

	uint8_t* ref = (uint8_t*)&len;
	for(size_t i = 0; i < sizeof(len); i++)
		buffer.push_back(ref[i]);

	for(size_t i = 0; i < len; i++)
		buffer.push_back(args[i]);

}

LIBAPI
BinarySerializer::SerializedType BinarySerializer::EndPack()
{
	return std::move(this->buffer);
}

LIBAPI
TFC::ServiceModel::BinaryDeserializer::BinaryDeserializer(SerializedType const& p) : bufferRef(p), currentPos(0)
{

}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Finalize()
{

}

template<>
LIBAPI
uint32_t TFC::ServiceModel::BinaryDeserializer::DeserializeImpl<uint32_t>()
{
	uint32_t tmp = 0;
	uint8_t* ref = (uint8_t*)&tmp;

	for(size_t i = 0; i < sizeof(tmp); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(tmp);
	return tmp;
}

template<>
LIBAPI
int64_t TFC::ServiceModel::BinaryDeserializer::DeserializeImpl<int64_t>()
{
	int64_t tmp = 0;
	uint8_t* ref = (uint8_t*)&tmp;

	for(size_t i = 0; i < sizeof(tmp); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(tmp);
	return tmp;
}

template<>
LIBAPI
double TFC::ServiceModel::BinaryDeserializer::DeserializeImpl<double>()
{
	double tmp = 0;
	uint8_t* ref = (uint8_t*)&tmp;

	for(size_t i = 0; i < sizeof(tmp); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(tmp);
	return tmp;
}

template<>
LIBAPI
std::string TFC::ServiceModel::BinaryDeserializer::DeserializeImpl<std::string>()
{
	decltype(std::declval<std::string>().length()) len = 0;
	uint8_t* ref = (uint8_t*)&len;

	for(size_t i = 0; i < sizeof(len); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(len);

	char* buf = new char[len + 1];
	buf[len] = '\0';

	for(size_t i = 0; i < len; i++)
		buf[i] = bufferRef[currentPos + i];

	std::string ret(buf);

	currentPos += len;

	delete[] buf;
	return ret;
}

template<>
LIBAPI
int TFC::ServiceModel::BinaryDeserializer::DeserializeImpl<int>()
{
	int tmp = 0;
	uint8_t* ref = (uint8_t*)&tmp;

	for(size_t i = 0; i < sizeof(tmp); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(tmp);
	return tmp;
}

template<>
LIBAPI
bool TFC::ServiceModel::BinaryDeserializer::DeserializeImpl<bool>()
{
	bool tmp = 0;
	uint8_t* ref = (uint8_t*)&tmp;

	for(size_t i = 0; i < sizeof(tmp); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(tmp);
	return tmp;
}

LIBAPI
void TFC::ServiceModel::BinarySerializer::Serialize(const SerializedType& p)
{
	if(p == this->buffer)
		return;


	auto len = p.size();

	for(size_t i = 0; i < len; i++)
		buffer.push_back(p[i]);

}
