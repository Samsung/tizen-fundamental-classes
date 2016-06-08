/*
 * REST.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include <curl/curl.h>
#include <exception>

#include "SRIN/Net/REST.h"
#include "SRIN/Net/Util.h"

#define REGEX_URLPARAM R"REGEX((\{([A-Za-z0-9_]+)\}))REGEX"
#include <regex>

#include <istream>
#include <sstream>
#include <vector>

using namespace SRIN::Net;

void RESTServiceTemplateBase::RegisterParameter(ParameterType paramType, CString key, IServiceParameter* ref)
{
	if (paramType == ParameterType::Header)
	{
		headerParam.push_back(std::make_pair(key, ref));
	}
	else if (paramType == ParameterType::Query)
	{
		queryStringParam.push_back(std::make_pair(key, ref));
	}
	else if (paramType == ParameterType::URL)
	{
		urlParam.insert(std::make_pair(std::string(key), ref));
	}
	else if(paramType == ParameterType::PostData)
	{
		postDataParam.insert(std::make_pair(std::string(key), ref));
	}
}

std::string RESTServiceTemplateBase::PreparePostData(const std::unordered_map<std::string, IServiceParameter*>& postDataParam)
{
	std::stringstream ret;

	bool first = true;
	for(auto& val : postDataParam)
	{
		if(!val.second->isSet)
			continue;

		if(!first)
			ret << "&";
		first = false;

		ret << val.first << "=" << val.second->GetEncodedValue();
	}

	return ret.str();
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
LIBAPI std::string SRIN::Net::GenericServiceParameter<std::string>::GetRawValue()
{
	return this->value;
}

template<>
LIBAPI std::string SRIN::Net::GenericServiceParameter<std::string>::GetEncodedValue()
{
	std::stringstream st;
	size_t len = this->value.length();
	auto str = this->value.c_str();

	for(int i = 0; i < len; i++)
	{
		if(isalnum(str[i]))
			st << str[i];
		else
			st << "%" << std::hex << ((int)str[i]);
	}
	return st.str();
}


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
	std::string combined = this->value.username + ":" + this->value.password;
	std::string encoded = "Basic " + Base64Encode((uint8_t*) combined.c_str(), combined.length());

	return encoded;
}

template<>
LIBAPI std::string SRIN::Net::BasicAuthParameter<ParameterType::Header>::GetEncodedValue()
{
	abort();
}

struct curl_slist* SRIN::Net::RESTServiceTemplateBase::PrepareHeader()
{
	struct curl_slist* headerList = nullptr;

	for (auto header : headerParam)
	{
		if (!header.second->isSet)
			continue;

		std::string headerString = header.first;
		headerString.append(": ");
		headerString.append(header.second->GetRawValue());
		headerList = curl_slist_append(headerList, headerString.c_str());
	}
	return headerList;
}

SRIN::Net::RESTServiceTemplateBase::RESTServiceTemplateBase(std::string url, HTTPMode httpMode) :
	UserAgent("srin-framework-tizen/1.0"),
	Url(url),
	working(false),
	httpMode(httpMode)
{
}

void SRIN::Net::RESTServiceTemplateBase::PrepareUrl()
{
	// Construct query string
	std::stringstream urlBuffer;

	// Resolve parameter on URL using regex
	std::regex urlRegex(REGEX_URLPARAM);

	std::string& text = this->Url;

	std::sregex_iterator words_begin(text.begin(), text.end(), urlRegex);
	auto words_end = std::sregex_iterator();

	auto lastToken = text.cbegin();

	for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
		std::smatch match = *i;

		// Flush to buffer
		auto tokenEnd = match[1].first;
		auto strCurr = std::string(lastToken, tokenEnd);
		lastToken = match[1].second;

		urlBuffer << strCurr;

		auto param = urlParam.find(match[2].str());
		if(param == urlParam.end())
		{
			dlog_print(DLOG_ERROR, "Parameter not found: %s", match[2].str().c_str());
			throw std::runtime_error("Parameter not found");
		}

		auto paramInstance = param->second;
		if(!paramInstance->isSet)
		{
			dlog_print(DLOG_ERROR, "Parameter not set: %s", param->first.c_str());
			throw std::runtime_error("Parameter not set");
		}
		urlBuffer << paramInstance->GetEncodedValue();
	}

	// Flush remaining url to buffer
	urlBuffer << std::string(lastToken, text.cend());

	if (queryStringParam.size())
	{
		urlBuffer << "?";
		bool first = true;
		for (auto queryString : queryStringParam)
		{
			dlog_print(DLOG_DEBUG, LOG_TAG, "Param: %s", queryString.first);

			if (!queryString.second->isSet)
				continue;

			if (first)
				first = false;
			else
				urlBuffer << "&";

			auto val = queryString.second->GetEncodedValue();


			urlBuffer << queryString.first << "=" << val;
		}
	}
	FinalUrl = urlBuffer.str();
	dlog_print(DLOG_DEBUG, LOG_TAG, "Final url: %s", FinalUrl.c_str());
}

class VectorWrapper: public std::streambuf
{
public:
	VectorWrapper(std::vector<char> &vec)
	{
		char* firstPtr = vec.data();
		char* endPtr = firstPtr + vec.size();

		this->setg(firstPtr, firstPtr, endPtr);
	}
};

size_t RESTServiceTemplateBase_WriteCallback(char *data, size_t size, size_t nmemb, void* d)
{
	auto str = reinterpret_cast<std::vector<char>*>(d);

	size_t realsize = nmemb * size;
	str->insert(str->end(), data, data + realsize);

	return realsize;
}

RESTResultBase SRIN::Net::RESTServiceTemplateBase::PerformCall()
{
	this->working = true;
	auto curlHandle = curl_easy_init();

	RESTResultBase returnObj;

	if (curlHandle)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "Prepare Header");

		// TODO make it an option so it can be configured
		curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 0);

		// Construct header list
		struct curl_slist* headerList = PrepareHeader();
		curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headerList);

		// Write function
		std::vector<char> buffer;

		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void* ) &buffer);
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, RESTServiceTemplateBase_WriteCallback);

		dlog_print(DLOG_DEBUG, LOG_TAG, "Prepare URL");

		// Construct query string
		PrepareUrl();
		curl_easy_setopt(curlHandle, CURLOPT_URL, FinalUrl.c_str());

		dlog_print(DLOG_DEBUG, LOG_TAG, "Prepare Post Data");

		// Prepare post data
		std::string postData;
		switch (httpMode)
		{
		case HTTPMode::Get:
			break;
		case HTTPMode::Post:
			curl_easy_setopt(curlHandle, CURLOPT_POST, 1L);
			goto HTTP_PreparePostData;
		case HTTPMode::Put:
			curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "PUT");

			HTTP_PreparePostData: postData = PreparePostData(postDataParam);

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
			returnObj.resultType = ResultType::LocalError;
			returnObj.errorMessage = err;
		}
		else
		{
			std::string response(buffer.data(), buffer.size());
			dlog_print(DLOG_DEBUG, LOG_TAG, "data: %s", response.c_str());
			curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &returnObj.httpCode);

			returnObj.responseObj = OnProcessResponseIntl(returnObj.httpCode, response, returnObj.errorCode,
				returnObj.errorMessage);

			if(returnObj.errorCode)
				returnObj.resultType = ResultType::ServerError;

		}
	}
	else
	{
		returnObj.resultType = ResultType::LocalError;
		returnObj.errorMessage = "Unknown error";
	}

	this->working = false;
	return returnObj;
}

std::string* SRIN::Net::SimpleRESTServiceBase::OnProcessResponse(int httpCode, const std::string& responseStr, int& errorCode, std::string& errorMessage)
{
	return nullptr;
}

SRIN::Net::RESTServiceTemplateBase::~RESTServiceTemplateBase()
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "REST service destroyed");
}

RESTResultBase SRIN::Net::RESTServiceTemplateBase::CallInternal()
{
	return PerformCall();
}

SRIN::Net::RESTResultBase::RESTResultBase() :
	resultType(ResultType::OK),
	responseObj(nullptr),
	httpCode(0),
	errorCode(0)
{
}

SRIN::Net::IServiceParameter::~IServiceParameter()
{
}
