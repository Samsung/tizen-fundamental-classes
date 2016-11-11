/*
 * REST.h
 *
 *  Created on: Feb 25, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#ifndef TFCFW_REST_H_
#define TFCFW_REST_H_

#include "TFC/Core.h"

#include <tuple>
#include <vector>
#include <unordered_map>

namespace TFC {
namespace Net {

/**
 * Enumeration for various HTTP request methods.
 */
enum class HTTPMode
{
	Unknown = 0, Get = 1, Post = 2, Put = 3, Delete = 4
};

/**
 * Enumeration for specifying type of request parameters.
 *
 * A parameter with type of Unknown will be ignored.
 * A parameter with type of Query will be used as GET/DELETE parameter.
 * A parameter with type of Header will be used in request header.
 * A parameter with type of PostData will be used as POST/PUT request body
 */
enum class ParameterType
{
	Unknown = 0, Query = 1, Header = 2, URL = 3, PostData = 4
};

/**
 * Enumeration for differentiating between success, server and local error.
 */
enum class ResultType
{
	OK = 0, ServerError = 1, LocalError = 2,
};

/**
 * Interface for parameter class.
 */
class LIBAPI IServiceParameter
{
protected:
	bool isSet;

	/**
	 * Method to get unmodified string value of the parameter object. Typically used for header parameters.
	 * Has to be overriden by the implementing parameter class.
	 *
	 * @return String that contains the raw value.
	 */
	virtual std::string GetRawValue() = 0;

	/**
	 * Method to get unmodified string value of the parameter object. Typically used for GET/POST parameters.
	 * Has to be overriden by the implementing parameter class.
	 *
	 * @return String that contains the encoded value.
	 */
	virtual std::string GetEncodedValue() = 0;

	friend class RESTServiceTemplateBase;
	virtual ~IServiceParameter();
};

/**
 * General template implementation of the IServiceParameter that holds a value.
 */
template<class Type>
class GenericServiceParameter: public IServiceParameter
{
protected:
	Type value;
	virtual std::string GetRawValue();
	virtual std::string GetEncodedValue();
};

/**
 * Base class for holding response from a REST call.
 */
class LIBAPI RESTResultBase
{
public:
	/**
	 * Constructor of RESTResultBase class.
	 */
	RESTResultBase();

	/**
	 * Indicate whether the result is success or not.
	 */
	ResultType resultType;

	/**
	 * Object to hold response from the server.
	 */
	void* responseObj;

	/**
	 * HTTP code of the response.
	 */
	int httpCode;

	/**
	 * Error code of the response.
	 */
	int errorCode;

	/**
	 * Error message from the server.
	 */
	std::string errorMessage;

	/**
	 * Enable access to private/protected member of base REST service class
	 */
	friend class RESTServiceTemplateBase;

};

/**
 * Template class that uses its template argument to provide type for responseObj.
 */
template<class T>
class RESTResult: public RESTResultBase, public PropertyClass<RESTResult<T>>
{
	template<typename TP>
	using Property = typename PropertyClass<RESTResult<T>>::template Property<TP>;
private:
	T* GetResponse()
	{
		auto var = reinterpret_cast<T*>(responseObj);
		return var;
	}

public:
	/**
	 * Default constructor of RESTResult class.
	 */
	RESTResult() : Response(this)
	{

	}

	/**
	 * Constructor of RESTResult that copy from RESTResultBase reference.
	 *
	 * @param p Reference to a RESTResultBase object.
	 */
	RESTResult(RESTResultBase&& p) : Response(this)
	{
		resultType = p.resultType;
		responseObj = p.responseObj;
		httpCode = p.httpCode;
		errorCode = p.errorCode;
		errorMessage = std::move(p.errorMessage);
	}

	/**
	 * Constructor of RESTResult that copy from RESTResultBase object.
	 *
	 * @param p Copy of a RESTResultBase object.
	 *//*
	RESTResult(RESTResultBase p) : Response(this)
	{
		resultType = p.resultType;
		responseObj = p.responseObj;
		httpCode = p.httpCode;
		errorCode = p.errorCode;
		errorMessage = std::move(p.errorMessage);
	}*/

	/**
	 * Copy constructor of RESTResult class.
	 *
	 * @param p Reference to a RESTResult object.
	 */
	RESTResult(RESTResult const& p) : Response(this)
	{
		resultType = p.resultType;
		responseObj = p.responseObj;
		httpCode = p.httpCode;
		errorCode = p.errorCode;
		errorMessage = p.errorMessage;
	}

	RESTResult& operator=(RESTResult&& p)
	{
		if (&p == this) return *this;

		resultType = p.resultType;
		responseObj = p.responseObj;
		httpCode = p.httpCode;
		errorCode = p.errorCode;
		errorMessage = std::move(p.errorMessage);
		return *this;
	}

	/**
	 * Property that enable getting pointer to responseObj.
	 */
	typename Property<T*>::template Get<&RESTResult::GetResponse> Response;

};

/**
 * Base template for REST Service.
 * It defines a Parameter class and handles request building.
 * It performs CURL call based on HTTPMethod and its parameter.
 */
class LIBAPI RESTServiceTemplateBase
{
public:
	template<ParameterType ParamType, class ValueType>
	/**
	 * Template class for parameters of a REST Service.
	 * The first type argument is ParameterType, used to determine its behavior.
	 * @see {ParameterType}
	 * The second type argument is the type of the value.
	 */
	class Parameter: public GenericServiceParameter<ValueType>
	{
	private:
		RESTServiceTemplateBase* instance;
		char const* key;
	public:
		/**
		 * Constructor for Parameter class. It will register the parameter to REST Service,
		 * but it won't be enabled as long as {operator=} is not called.
		 *
		 * @param instance REST Service object pointer that owns the parameter.
		 * @param key Constant string that will be copied as key of the parameter.
		 */
		Parameter(RESTServiceTemplateBase* instance, char const* key) :
			key(key), instance(instance)
		{
			this->isSet = false;
			instance->RegisterParameter(ParamType, key, this);
		}
		/**
		 * Method that overload operator = to copy value and "set" them.
		 * Using = is the proper method to enable the parameter for a REST service.
		 *
		 * @param val ValueType reference that will be copied as parameter value.
		 */
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

	virtual std::string PreparePostData(const std::unordered_map<std::string, IServiceParameter*>& postDataParam);
	virtual void PreprocessPostData(std::string& postData) {}

	std::string UserAgent, Url, FinalUrl;
	RESTResultBase CallInternal();
private:
	virtual void* OnProcessResponseIntl(int httpCode, const std::string& responseStr, int& errorCode,
		std::string& errorMessage) = 0;
	virtual void* OnProcessErrorIntl(int httpCode, int& errorCode, std::string& errorMessage)
	{
		return nullptr;
	}
	RESTResultBase PerformCall();
	void RegisterParameter(ParameterType paramType, char const* key, IServiceParameter* ref);

	bool working;
	HTTPMode httpMode;

	struct curl_slist* PrepareHeader();
	void PrepareUrl();

	std::vector<std::pair<char const*, IServiceParameter*>> queryStringParam;
	std::vector<std::pair<char const*, IServiceParameter*>> headerParam;
	std::unordered_map<std::string, IServiceParameter*> urlParam;
	std::unordered_map<std::string, IServiceParameter*> postDataParam;
};

/**
 * Base class for a REST Service.
 * This is the class that you want to inherit if you want to make a REST Service.
 */
template<class ResponseType>
class RESTServiceBase: public RESTServiceTemplateBase
{
private:
	virtual void* OnProcessResponseIntl(int httpCode, const std::string& responseStr, int& errorCode,
		std::string& errorMessage)
	{
		return OnProcessResponse(httpCode, responseStr, errorCode, errorMessage);
	}
	virtual void* OnProcessErrorIntl(int httpCode, int& errorCode, std::string& errorMessage)
	{
		return OnProcessError(httpCode, errorCode, errorMessage);
	}
protected:
	/**
	 * Constructor for RESTServiceBase.
	 * You have to define HTTP Mode and URL here.
	 *
	 * @param url The base URL of the request.
	 * @param httpMode HTTP Mode of the request.
	 */
	RESTServiceBase(std::string url, HTTPMode httpMode) :
		RESTServiceTemplateBase(url, httpMode)
	{
	}

	/**
	 * Method that you have to implement to handle response from the server. Non-optional.
	 *
	 * @param httpCode HTTP code of the response.
	 * @param responseStr String reference of the response body.
	 * @param errorCode Error code of the response.
	 * @param errorMessage Error message of the response.
	 *
	 * @return Pointer to the resulting object with the corresponding type provided from template argument.
	 */

	virtual ResponseType* OnProcessResponse(int httpCode, const std::string& responseStr, int& errorCode,
		std::string& errorMessage) = 0;
	/**
	 * Method that you have to implement to handle failure. Optional.
	 *
	 * @param httpCode HTTP code of the response.
	 * @param errorCode Error code of the response.
	 * @param errorMessage Error message of the response.
	 *
	 * @return Pointer to the resulting object with the corresponding type provided from template argument.
	 */
	virtual ResponseType* OnProcessError(int httpCode, int& errorCode, std::string& errorMessage)
	{
		return nullptr;
	}
public:
	/**
	 * Perform the REST request.
	 */
	RESTResult<ResponseType> Call()
	{
		return std::move(CallInternal());
	}

	/**
	 * Destructor of RESTServiceBase.
	 */
	virtual ~RESTServiceBase()
	{

	}
};

/**
 * Sample implementation of REST Service that returns string on call.
 */
class SimpleRESTServiceBase: public RESTServiceBase<std::string>
{
protected:
	/**
	 * Constructor of SimpleRESTServiceBase, use HTTPMode Get.
	 *
	 * @param url The base URL of the request.
	 */
	SimpleRESTServiceBase(std::string url) :
		RESTServiceBase(url, HTTPMode::Get)
	{
	}

	/**
	 * Implement OnProcessResponse that returns string.
	 *
	 * @param httpCode HTTP code of the response.
	 * @param responseStr String reference of the response body.
	 * @param errorCode Error code of the response.
	 * @param errorMessage Error message of the response.
	 *
	 * @return Pointer to the resulting string.
	 */
	virtual std::string* OnProcessResponse(int httpCode, const std::string& responseStr, int& errorCode, std::string& errorMessage);
};

/**
 * Structure of basic authentication data.
 */
struct BasicAuthAccount
{
	std::string username;
	std::string password;
};

/**
 * Special class of Parameter that handles basic access authentication for HTTP request.
 */
template<ParameterType ParamType>
class BasicAuthParameter: public RESTServiceTemplateBase::Parameter<ParamType, BasicAuthAccount>
{
public:
	/**
	 * Implement Parameter constructor with "Authorization" field name as key.
	 *
	 * @param instance REST Service object pointer that owns the parameter.
	 */
	BasicAuthParameter(RESTServiceTemplateBase* instance) :
		RESTServiceTemplateBase::Parameter<ParamType, BasicAuthAccount>(instance, "Authorization")
	{
	}
	/**
	 * Implement operator= to copy BasicAuthAccount struct.
	 *
	 * @param val BasicAuthAccount reference that will be copied as parameter value.
	 */
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
