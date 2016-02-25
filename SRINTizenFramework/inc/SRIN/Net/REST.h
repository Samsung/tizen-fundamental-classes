/*
 * REST.h
 *
 *  Created on: Feb 25, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com) M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef REST_H_
#define REST_H_

#include "SRIN/Core.h"

namespace SRIN { namespace Net {

	enum class HTTPMode
	{
		HTTP_UNKNOWN = 0,
		HTTP_GET = 1,
		HTTP_POST = 2,
		HTTP_PUT = 3,
		HTTP_DELETE = 4
	};

	enum class ParameterType
	{
		PARAMETER_UNKNOWN = 0,
		PARAMETER_QUERY = 1,
		PARAMETER_HEADER = 2,
	};

	class RequestContext
	{
	public:
		void AddQueryString(std::string& key, std::string& value);
		void AddHeaderEntry(std::string& key, std::string& value);
		void AddPostData(std::string& data);
	};

	class RESTServiceTemplateBase
	{
	public:
		Event OnSuccess;
		Event OnFailed;
	protected:
		virtual std::string PreparePostData();

	private:
		virtual void* OnProcessResponseIntl(std::string& responseStr);
		void PerformCall();

		friend class Parameter;
		void RegisterParameter(ParameterType paramType, CString key, std::string* ref);

		bool working;
		std::string url;
		HTTPMode httpMode;
	};

	template<class ResponseType>
	class RESTServiceBase : public RESTServiceTemplateBase
	{
	private:
		virtual void* OnProcessResponseIntl(std::string& responseStr) { return OnProcessResponse(responseStr); }
	protected:
		virtual ResponseType* OnProcessResponse(std::string& responseStr) { return nullptr; }
	};

	template<class ServiceClass, CString Key, ParameterType ParamType>
	class Parameter
	{
	private:
		std::string value;
	public:
		Parameter(ServiceClass* instance) { instance->RegisterParameter(ParamType, Key, &value); }

	};



}}



#endif /* REST_H_ */
