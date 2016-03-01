/*
 * REST.h
 *
 *  Created on: Feb 25, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com) M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef REST_H_
#define REST_H_

#include "SRIN/Core.h"

#include <tuple>
#include <vector>



namespace SRIN { namespace Net {

	enum class HTTPMode
	{
		Unknown = 0,
		Get = 1,
		Post = 2,
		Put = 3,
		Delete = 4
	};

	enum class ParameterType
	{
		Unknown = 0,
		Query = 1,
		Header = 2,
	};


	class LIBAPI IServiceParameter
	{
	protected:
		virtual std::string GetRawValue() = 0;
		virtual std::string GetEncodedValue() = 0;

		friend class RESTServiceTemplateBase;
	};

	template<class Type>
	class GenericServiceParameter : public IServiceParameter
	{
	protected:
		Type value;
		virtual std::string GetRawValue();
		virtual std::string GetEncodedValue();
	};



	class LIBAPI RESTServiceTemplateBase
	{
	public:
		template<ParameterType ParamType, class ValueType>
		class Parameter : public GenericServiceParameter<ValueType>
		{
		private:
			RESTServiceTemplateBase* instance;
			CString key;
		public:
			Parameter(RESTServiceTemplateBase* instance, CString key) : key(key), instance(instance) { instance->RegisterParameter(ParamType, key, this); }
			void operator=(const ValueType& val) { if(!instance->working) this->value = val; }
		};

		Event<> OnSuccess;
		Event<> OnFailed;

		void CallAsync();
	protected:
		RESTServiceTemplateBase(std::string url, HTTPMode httpMode);

		virtual std::string PreparePostData();
		std::string UserAgent, Url;

	private:
		virtual void* OnProcessResponseIntl(const std::string& responseStr) = 0;
		void PerformCall();
		void RegisterParameter(ParameterType paramType, CString key, IServiceParameter* ref);

		bool working;
		HTTPMode httpMode;
		void* result;



		struct curl_slist* PrepareHeader();
		std::string PrepareUrl();

		std::vector<std::pair<CString, IServiceParameter*>> queryStringParam;
		std::vector<std::pair<CString, IServiceParameter*>> headerParam;
	};


	template<class ResponseType>
	class RESTServiceBase : public RESTServiceTemplateBase
	{
	private:
		virtual void* OnProcessResponseIntl(const std::string& responseStr) { return OnProcessResponse(responseStr); }
	protected:
		RESTServiceBase(std::string url, HTTPMode httpMode) : RESTServiceTemplateBase(url, httpMode) { }
		virtual ResponseType* OnProcessResponse(const std::string& responseStr) = 0;
	};

	class SimpleRESTServiceBase : public RESTServiceBase<std::string>
	{
	protected:
		SimpleRESTServiceBase(std::string url) : RESTServiceBase(url, HTTPMode::Get) { }
		virtual std::string* OnProcessResponse(const std::string& responseStr);
	};


	struct BasicAuthAccount
	{
		std::string username;
		std::string password;
	};

	template<ParameterType ParamType>
	class BasicAuthParameter : public RESTServiceTemplateBase::Parameter<ParamType, BasicAuthAccount>
	{
	public:
		BasicAuthParameter(RESTServiceTemplateBase* instance) : RESTServiceTemplateBase::Parameter<ParamType, BasicAuthAccount>(instance, "Authorization") { }
		void operator=(const BasicAuthAccount& val) { RESTServiceTemplateBase::Parameter<ParamType, BasicAuthAccount>::operator=(val); }
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
	GSPDeclare(int8_t);
	GSPDeclare(uint8_t);
	GSPDeclare(int16_t);
	GSPDeclare(uint16_t);
	GSPDeclare(int32_t);
	GSPDeclare(uint32_t);
	GSPDeclare(int64_t);
	GSPDeclare(uint64_t);
	GSPDeclare(float);
	GSPDeclare(double);
	GSPDeclare(std::string);
	GSPDeclare(BasicAuthAccount);
#	undef GSPDeclare



	}
}



#endif /* REST_H_ */
