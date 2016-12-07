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
#include <map>

namespace TFC {
namespace Net {

	struct AuthHeader {
		std::map<std::string, std::string> headers;
		std::string consumerSecret;
		AuthHeader(	std::string const& consumerKey,
					std::string const& consumerSecret,
					std::string const& token = "",
					std::string const& oAuthCallback = "");
	};

	std::string CalculateSignature(std::string const& base, AuthHeader& authHeader);

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
	public:
		template<ParameterType P, typename T>
		using Parameter = typename TFC::Net::RESTServiceBase<ResponseType>::template Parameter<P, T>;

		OAuthRESTServiceTemplateBase(std::string url, HTTPMode httpMode, OAuthParam* param,
				std::string const& token = "", std::string const& oAuthCallback = "");

		virtual ~OAuthRESTServiceTemplateBase() {}
	protected:
		virtual void OnBeforePrepareRequest();
	private:
		std::string token;
		std::string oAuthCallback;
		OAuthParam* paramPtr;
		//typedef ClientInfoExtractor<TClientInfoProvider> ClientInfo;
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
TFC::Net::OAuthRESTServiceTemplateBase<ResponseType>::OAuthRESTServiceTemplateBase(std::string url, HTTPMode httpMode, OAuthParam* param,
		std::string const& token, std::string const& oAuthCallback) :
		TFC::Net::RESTServiceBase<ResponseType>(url, httpMode),
		token(token),
		paramPtr(param),
		oAuthCallback(oAuthCallback),
		Authorization(this, "Authorization")
{
}

template<class ResponseType>
void TFC::Net::OAuthRESTServiceTemplateBase<ResponseType>::OnBeforePrepareRequest()
{
	AuthHeader authHeader(paramPtr->clientId, paramPtr->clientSecret, token, oAuthCallback);
	std::string base = CreateSignatureBaseString(this->Url, this->httpMode, this->postDataParam, this->queryStringParam, authHeader);
	std::string signature = CalculateSignature(base, authHeader);

	std::stringstream authString;
	authString << "OAuth ";
	bool first = true;
	for (auto& kv : authHeader.headers)
	{
		if (!kv.second.empty())
		{
			if (!first)
			{
				authString << ",";
			}
			first = false;
			authString << kv.first << "=\"" << kv.second << "\"";
		}
	}

	Authorization = authString.str();
}

#endif /* TFC_NET_OAUTHREST_H_ */
