/*
 * REST.h
 *
 *  Created on: Feb 25, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com) M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRINFW_REST_H_
#define SRINFW_REST_H_

#include "SRIN/Core.h"

#include <tuple>
#include <vector>
#include <unordered_map>

namespace SRIN {
namespace Net {

enum class HTTPMode
{
	Unknown = 0, Get = 1, Post = 2, Put = 3, Delete = 4
};

enum class ParameterType
{
	Unknown = 0, Query = 1, Header = 2, URL = 3
};

enum class ResultType
{
	OK = 0, ServerError = 1, LocalError = 2,
};

class LIBAPI IServiceParameter
{
protected:
	bool isSet;
	virtual std::string GetRawValue() = 0;
	virtual std::string GetEncodedValue() = 0;
	friend class RESTServiceTemplateBase;
	virtual ~IServiceParameter();
};

template<class Type>
class GenericServiceParameter: public IServiceParameter
{
protected:
	Type value;
	virtual std::string GetRawValue();
	virtual std::string GetEncodedValue();
};

class LIBAPI RESTResultBase
{
public:
	RESTResultBase();
	ResultType resultType;
	void* responseObj;
	int httpCode;
	int errorCode;
	std::string errorMessage;
	friend class RESTServiceTemplateBase;

};

template<class T>
class RESTResult: public RESTResultBase, public PropertyClass
{

private:
	T* GetResponse()
	{
		auto var = reinterpret_cast<T*>(responseObj);
		return var;
	}

public:
	RESTResult()
	{

	}

	RESTResult(RESTResultBase& p) : Response(this)
	{
		resultType = p.resultType;
		responseObj = p.responseObj;
		httpCode = p.httpCode;
		errorCode = p.errorCode;
		errorMessage = std::move(p.errorMessage);
	}

	RESTResult(RESTResultBase p) : Response(this)
	{
		resultType = p.resultType;
		responseObj = p.responseObj;
		httpCode = p.httpCode;
		errorCode = p.errorCode;
		errorMessage = std::move(p.errorMessage);
	}

	RESTResult(RESTResult& p) : Response(this)
	{
		resultType = p.resultType;
		responseObj = p.responseObj;
		httpCode = p.httpCode;
		errorCode = p.errorCode;
		errorMessage = std::move(p.errorMessage);
	}

	typename Property<RESTResult, T*>::template Get<&RESTResult::GetResponse> Response;

};

class LIBAPI RESTServiceTemplateBase
{
public:
	template<ParameterType ParamType, class ValueType>
	class Parameter: public GenericServiceParameter<ValueType>
	{
	private:
		RESTServiceTemplateBase* instance;
		CString key;
	public:
		Parameter(RESTServiceTemplateBase* instance, CString key) :
			key(key), instance(instance)
		{
			this->isSet = false;
			instance->RegisterParameter(ParamType, key, this);
		}
		void operator=(const ValueType& val)
		{
			if (!instance->working)
			{
				this->isSet = true;
				this->value = val;
			}
		}
	};

	virtual ~RESTServiceTemplateBase();

protected:
	RESTServiceTemplateBase(std::string url, HTTPMode httpMode);

	virtual std::string PreparePostData();
	std::string UserAgent, Url;
	RESTResultBase CallInternal();
private:
	virtual void* OnProcessResponseIntl(int httpCode, const std::string& responseStr, int& errorCode,
		std::string& errorMessage) = 0;
	RESTResultBase PerformCall();
	void RegisterParameter(ParameterType paramType, CString key, IServiceParameter* ref);

	bool working;
	HTTPMode httpMode;

	struct curl_slist* PrepareHeader();
	std::string PrepareUrl();

	std::vector<std::pair<CString, IServiceParameter*>> queryStringParam;
	std::vector<std::pair<CString, IServiceParameter*>> headerParam;
	std::unordered_map<std::string, IServiceParameter*> urlParam;
};

template<class ResponseType>
class RESTServiceBase: public RESTServiceTemplateBase
{
private:
	virtual void* OnProcessResponseIntl(int httpCode, const std::string& responseStr, int& errorCode,
		std::string& errorMessage)
	{
		return OnProcessResponse(httpCode, responseStr, errorCode, errorMessage);
	}
protected:
	RESTServiceBase(std::string url, HTTPMode httpMode) :
		RESTServiceTemplateBase(url, httpMode)
	{
	}
	virtual ResponseType* OnProcessResponse(int httpCode, const std::string& responseStr, int& errorCode,
		std::string& errorMessage) = 0;
public:
	RESTResult<ResponseType> Call()
	{
		return RESTResult<ResponseType>(CallInternal());
	}

	virtual ~RESTServiceBase()
	{

	}
};

class SimpleRESTServiceBase: public RESTServiceBase<std::string>
{
protected:
	SimpleRESTServiceBase(std::string url) :
		RESTServiceBase(url, HTTPMode::Get)
	{
	}
	virtual std::string* OnProcessResponse(int httpCode, const std::string& responseStr, int& errorCode, std::string& errorMessage);
};

struct BasicAuthAccount
{
	std::string username;
	std::string password;
};

template<ParameterType ParamType>
class BasicAuthParameter: public RESTServiceTemplateBase::Parameter<ParamType, BasicAuthAccount>
{
public:
	BasicAuthParameter(RESTServiceTemplateBase* instance) :
		RESTServiceTemplateBase::Parameter<ParamType, BasicAuthAccount>(instance, "Authorization")
	{
	}
	void operator=(const BasicAuthAccount& val)
	{
		RESTServiceTemplateBase::Parameter<ParamType, BasicAuthAccount>::operator=(val);
	}
	virtual ~BasicAuthParameter()
	{
	}
protected:
	virtual std::string GetRawValue();
	virtual std::string GetEncodedValue();

};

template<>
std::string BasicAuthParameter<ParameterType::Query>::GetRawValue();

template<>
std::string BasicAuthParameter<ParameterType::Query>::GetEncodedValue();

template<>
std::string BasicAuthParameter<ParameterType::Header>::GetRawValue();

template<>
std::string BasicAuthParameter<ParameterType::Header>::GetEncodedValue();

/*
 * Specialization Definition
 */

#	define GSPDeclare(TYPENAME) \
			template<> std::string GenericServiceParameter< TYPENAME >::GetRawValue(); \
			template<> std::string GenericServiceParameter< TYPENAME >::GetEncodedValue()
GSPDeclare(int8_t);GSPDeclare(uint8_t);GSPDeclare(int16_t);GSPDeclare(uint16_t);GSPDeclare(int32_t);GSPDeclare(uint32_t);GSPDeclare(int64_t);GSPDeclare(uint64_t);GSPDeclare(float);GSPDeclare(double);GSPDeclare(std::string);GSPDeclare(BasicAuthAccount);
#	undef GSPDeclare

}}

#endif /* REST_H_ */
