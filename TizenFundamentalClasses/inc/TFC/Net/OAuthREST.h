/*
 * OAuthREST.h
 *
 *  Created on: Nov 29, 2016
 *      Author: gilang
 */

#ifndef TFC_NET_OAUTHREST_H_
#define TFC_NET_OAUTHREST_H_

#include "TFC/Net/REST.h"
#include "TFC/Net/OAuth.h"
#include <sstream>

namespace TFC {
namespace Net {

	struct AuthHeader {
		std::unordered_map<std::string, std::string> headers;

		AuthHeader(	std::string const& consumerKey,
					std::string const& token = "",
					std::string const& oAuthCallback = "");
	};

	std::string CalculateSignature(std::string const& base, AuthHeader const& authHeader);

	std::string CreateSignatureBaseString(
			std::string const& url, HTTPMode httpMode,
			std::unordered_map<std::string, IServiceParameter*> const& postDataParam,
			std::vector<std::pair<char const*, IServiceParameter*>> const& queryStringParam,
			AuthHeader const& authHeader);


	/**
	 *
	 */
	template<class ResponseType>
	class OAuthRESTServiceTemplateBase : public TFC::Net::RESTServiceBase<ResponseType>
	{
		template<ParameterType P, typename T>
		using Parameter = typename TFC::Net::RESTServiceBase<ResponseType>::template Parameter<P, T>;
	public:
		OAuthRESTServiceTemplateBase(std::string url, HTTPMode httpMode, OAuthParam* param, std::string const& token = "");

		virtual ~OAuthRESTServiceTemplateBase() {}
	protected:
		virtual void OnBeforePrepareRequest();
	private:
		std::string token;
		OAuthParam* paramPtr;
		//typedef UserInfoExtractor<TClientInfoProvider> ClientInfo;
		Parameter<TFC::Net::ParameterType::Header, std::string> Authorization;
	};


	/**
	 *
	 */
	template<typename TClientInfoProvider, class ResponseType>
	class OAuthRESTServiceBase : public TFC::Net::OAuthRESTServiceTemplateBase<ResponseType>
	{
	public:
		OAuthRESTServiceBase(std::string url, HTTPMode httpMode, std::string const& token = "") :
			OAuthRESTServiceTemplateBase<ResponseType>(url, httpMode, OAuthParam::BuildClientInfo<TClientInfoProvider>(), token)
		{

		}
	};
}
}

template<class ResponseType>
TFC::Net::OAuthRESTServiceTemplateBase<ResponseType>::OAuthRESTServiceTemplateBase(std::string url, HTTPMode httpMode, OAuthParam* param, const std::string& token) :
		TFC::Net::RESTServiceBase<ResponseType>(url, httpMode),
		token(token),
		paramPtr(param),
		Authorization(this, "Authorization")
{

}

template<class ResponseType>
void TFC::Net::OAuthRESTServiceTemplateBase<ResponseType>::OnBeforePrepareRequest()
{
	AuthHeader authHeader(paramPtr->clientId);
	std::string base = CreateSignatureBaseString(this->Url, this->httpMode, this->postDataParam, this->queryStringParam, authHeader);
	std::string signature = CalculateSignature(base, authHeader);

	std::stringstream authString;
	authString << "OAuth ";
	for (auto& kv : authHeader.headers)
	{
		authString << kv.first << "=" << kv.second << ",";
	}
	authString << "oauth_signature" << "=" << signature;

	Authorization = authString.str();
}

#endif /* TFC_NET_OAUTHREST_H_ */
