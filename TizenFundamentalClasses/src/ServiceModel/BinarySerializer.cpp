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
 *    ServiceModel/BinarySerializer.cpp
 *
 * Created on:  Jan 30, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/ServiceModel/BinarySerializer.h"

using namespace TFC::ServiceModel;

LIBAPI
BinarySerializer::BinarySerializer() {
	buffer = new SerializedType;
	doDestruction = true;
}

LIBAPI
void BinarySerializer::Serialize(uint32_t args)
{
	dlog_print(DLOG_DEBUG, "BinSer", "Serialize uint32_t %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);
}

LIBAPI
void BinarySerializer::Serialize(int64_t args)
{
	dlog_print(DLOG_DEBUG, "BinSer", "Serialize int64_t %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);

}

LIBAPI
void BinarySerializer::Serialize(uint64_t args)
{
	dlog_print(DLOG_DEBUG, "BinSer", "Serialize uint64_t %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);

}

LIBAPI
void BinarySerializer::Serialize(int args)
{
	dlog_print(DLOG_DEBUG, "BinSer", "Serialize int32_t %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);
}

LIBAPI
void BinarySerializer::Serialize(bool args)
{
	dlog_print(DLOG_DEBUG, "BinSer", "Serialize bool %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);
}

LIBAPI
void BinarySerializer::Serialize(double args)
{
	dlog_print(DLOG_DEBUG, "BinSer", "Serialize double %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);
}

LIBAPI
void BinarySerializer::Serialize(std::string const& args)
{
	dlog_print(DLOG_DEBUG, "BinSer", "Serialize string %s", args.c_str());
	auto len = args.length();

	uint8_t* ref = (uint8_t*)&len;
	for(size_t i = 0; i < sizeof(len); i++)
		buffer->push_back(ref[i]);

	for(size_t i = 0; i < len; i++)
		buffer->push_back(args[i]);

}

LIBAPI
BinarySerializer::SerializedType BinarySerializer::EndPack()
{
	return std::move(*buffer);
}

LIBAPI
TFC::ServiceModel::BinaryDeserializer::BinaryDeserializer(SerializedType const& p) : bufferRef(p), currentPos(0)
{

}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Finalize()
{

}
/*
template<>
LIBAPI
uint32_t TFC::ServiceModel::BinaryDeserializer::DeserializeImpl<uint32_t>()
{

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
*/
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
/*
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
*/

/*
LIBAPI
void TFC::ServiceModel::BinarySerializer::Serialize(const SerializedType& p)
{
	if(p == this->buffer)
		return;

	dlog_print(DLOG_DEBUG, "BinSer", "Serialize serialized bytes size: %d", p.size());

	auto len = p.size();

	for(size_t i = 0; i < len; i++)
		buffer->push_back(p[i]);

}
*/

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(int8_t& target)
{
	target = DeserializeImpl<int8_t>();
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(int16_t& target)
{
	target = DeserializeImpl<int16_t>();
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(int32_t& target)
{
	target = DeserializeImpl<int32_t>();
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(int64_t& target)
{
	target = DeserializeImpl<int64_t>();
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(uint8_t& target)
{
	target = DeserializeImpl<uint8_t>();
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(uint16_t& target)
{
	target = DeserializeImpl<uint16_t>();
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(uint32_t& target)
{
	target = DeserializeImpl<uint32_t>();
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(uint64_t& target)
{
	target = DeserializeImpl<uint64_t>();
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(std::string& target)
{
	target = DeserializeImpl<std::string>();
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(bool& target)
{
	target = DeserializeImpl<bool>();
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(double& target)
{
	target = DeserializeImpl<double>();
}


LIBAPI
BinarySerializer TFC::ServiceModel::BinarySerializer::CreateScope()
{
	return { this->buffer };
}

LIBAPI
void TFC::ServiceModel::BinarySerializer::Serialize(BinarySerializer& p) {
	// Just do nothing as the scope is already pushed on the buffer
}

LIBAPI
TFC::ServiceModel::BinarySerializer::~BinarySerializer()
{
	if(buffer && doDestruction)
		delete buffer;
}

LIBAPI
TFC::ServiceModel::BinarySerializer::BinarySerializer(
		SerializedType* bufferRef) {
	this->buffer = bufferRef;
	this->doDestruction = false;
}

LIBAPI
void TFC::ServiceModel::BinarySerializer::Serialize(unsigned char args)
{
	this->buffer->push_back(args);
}

LIBAPI
void TFC::ServiceModel::BinarySerializer::Serialize(const std::vector<uint8_t>& args)
{
	if (args.empty())
	{
		Serialize((uint32_t)0);
	}
	else
	{
		Serialize(args.size());

		for (auto obj : args)
		{
			Serialize(obj);
		}
	}
}

LIBAPI
void TFC::ServiceModel::BinaryDeserializer::Deserialize(std::vector<uint8_t>& target)
{
	target.clear();

	uint32_t size = 0;
	Deserialize(size);

	for(int i = 0; i < size; i++)
	{
		uint8_t tmp = 0;
		Deserialize(tmp);
		target.push_back(tmp);
	}
}
