/*
 * REST.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */


#include <curl/curl.h>

#include "SRIN/Net/REST.h"



#include <istream>
#include <sstream>
#include <vector>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
typedef boost::iostreams::stream<boost::iostreams::array_source> ResponseBuffer;

using namespace SRIN::Net;

void RESTServiceTemplateBase::RegisterParameter(ParameterType paramType, CString key, IServiceParameter* ref)
{
	if(paramType == ParameterType::Header)
	{
		headerParam.push_back(std::make_pair(key, ref));
	}
	else if(paramType == ParameterType::Query)
	{
		queryStringParam.push_back(std::make_pair(key, ref));
	}
}


std::string RESTServiceTemplateBase::PreparePostData()
{
	return std::string();
}


/*
 * GenericServiceParameter Template Definiition
 */

#define GSPDefineRaw(TYPENAME) \
		template<> __attribute__((__visibility__("default"))) \
		std::string GenericServiceParameter< TYPENAME >::GetRawValue()

#define GSPDefineEncoded(TYPENAME) \
		template<> __attribute__((__visibility__("default"))) \
		std::string GenericServiceParameter< TYPENAME >::GetEncodedValue()

#define GSPDefineDefault(TYPENAME) \
		template<> __attribute__((__visibility__("default"))) \
		std::string GenericServiceParameter< TYPENAME >::GetRawValue() \
		{ return std::to_string(this->value); } \
		template<> __attribute__((__visibility__("default"))) \
		std::string GenericServiceParameter< TYPENAME >::GetEncodedValue() \
		{ return std::to_string(this->value); }

GSPDefineDefault(int8_t);
GSPDefineDefault(uint8_t);
GSPDefineDefault(int16_t);
GSPDefineDefault(uint16_t);
GSPDefineDefault(int32_t);
GSPDefineDefault(uint32_t);
GSPDefineDefault(int64_t);
GSPDefineDefault(uint64_t);
GSPDefineDefault(float);
GSPDefineDefault(double);



template<>
LIBAPI std::string GenericServiceParameter<BasicAuthAccount>::GetRawValue()
{
	abort(); // The application must never reach this place
}

template<>
LIBAPI
std::string GenericServiceParameter<BasicAuthAccount>::GetEncodedValue()
{
	abort(); // The application must never reach this place
}


template<>
LIBAPI std::string SRIN::Net::BasicAuthParameter<ParameterType::Query>::GetRawValue()
{
	return std::string();
}

template<>
LIBAPI std::string SRIN::Net::BasicAuthParameter<ParameterType::Query>::GetEncodedValue()
{
	return std::string();
}

template<>
LIBAPI std::string SRIN::Net::BasicAuthParameter<ParameterType::Header>::GetRawValue()
{
	return std::string();
}

template<>
LIBAPI std::string SRIN::Net::BasicAuthParameter<ParameterType::Header>::GetEncodedValue()
{
	return std::string();
}

struct curl_slist* SRIN::Net::RESTServiceTemplateBase::PrepareHeader()
{
	struct curl_slist* headerList = nullptr;

	for (auto header : headerParam)
	{
		std::string headerString = header.first;
		headerString.append(": ");
		headerString.append(header.second->GetRawValue());
		headerList = curl_slist_append(headerList, headerString.c_str());
	}
	return headerList;
}

SRIN::Net::RESTServiceTemplateBase::RESTServiceTemplateBase(std::string url, HTTPMode httpMode) :
	Url(url), result(nullptr), UserAgent("srin-framework-tizen/1.0"), httpMode(httpMode)
{
}

std::string SRIN::Net::RESTServiceTemplateBase::PrepareUrl()
{
	// Construct query string
	std::stringstream urlBuffer;
	urlBuffer << this->Url;
	if (queryStringParam.size())
	{
		urlBuffer << "?";
		bool first = true;
		for (auto queryString : queryStringParam)
		{
			if (first)
				first = false;
			else
				urlBuffer << "+";

			urlBuffer << queryString.first << "=" << queryString.second->GetEncodedValue();
		}
	}
	std::string finalUrl = urlBuffer.str();
	return finalUrl;
}

class VectorWrapper : public std::streambuf {
public:
	VectorWrapper(std::vector<char> &vec) {
		char* firstPtr = vec.data();
		char* endPtr = firstPtr + vec.size();

        this->setg(firstPtr, firstPtr, endPtr);
    }
};

size_t RESTServiceTemplateBase_WriteCallback(char *data, size_t size, size_t nmemb, void* d)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Buffer Write...");

	auto str = reinterpret_cast<std::vector<char>*>(d);

	size_t realsize = nmemb * size;
	str->insert(str->end(), data, data + realsize);

	return realsize;
}


void SRIN::Net::RESTServiceTemplateBase::PerformCall()
{
	auto curlHandle = curl_easy_init();

	if(curlHandle)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "Prepare Header");

		// Construct header list
		struct curl_slist* headerList = PrepareHeader();
		curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headerList);

		// Write function
		std::vector<char> buffer;

		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void*) &buffer);
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION,
			RESTServiceTemplateBase_WriteCallback
		);


		dlog_print(DLOG_DEBUG, LOG_TAG, "Prepare URL");

		// Construct query string
		std::string finalUrl = PrepareUrl();
		curl_easy_setopt(curlHandle, CURLOPT_URL, finalUrl.c_str());

		auto urlBuf = finalUrl.c_str();

		dlog_print(DLOG_DEBUG, LOG_TAG, "Prepare Post Data");

		// Prepare post data
		std::string postData;
		switch(httpMode)
		{
		case HTTPMode::Get:
			break;
		case HTTPMode::Post:
			curl_easy_setopt(curlHandle, CURLOPT_POST, 1L);
			goto HTTP_PreparePostData;
		case HTTPMode::Put:
			curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "PUT");

			HTTP_PreparePostData:
			postData = PreparePostData();

			curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDSIZE, postData.size());
			curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, postData.c_str());
			break;
		case HTTPMode::Delete:
			curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "DELETE");
			break;
		}

		// USer Agent
		curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, UserAgent.c_str());

		dlog_print(DLOG_DEBUG, LOG_TAG, "Before Sending");

		CURLcode res = curl_easy_perform(curlHandle);
		dlog_print(DLOG_DEBUG, LOG_TAG, "After Sending");

		if (res != CURLE_OK)
		{
			auto err = curl_easy_strerror(res);
		}
		else
		{
			std::string response(buffer.data(), buffer.size());
			auto responseObj = OnProcessResponseIntl(response);
		}
	}
}

std::string* SRIN::Net::SimpleRESTServiceBase::OnProcessResponse(const std::string& responseStr)
{
}

void SRIN::Net::RESTServiceTemplateBase::CallAsync()
{
	PerformCall();
}
