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
 *    Net/OAuthREST.h
 *
 * Helper classes to access REST service using OAuth authorization protocol
 *
 * Created on: 	Nov 29, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
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

	std::string CalculateSignature(std::string const& base, AuthHeader& authHeader, std::string const& tokenSecret = "");

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
		OAuthParam* paramPtr;
		std::string token;
	private:
		std::string oAuthCallback;
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

	/**
	 *
	 */
	template<typename TClientInfoProvider, class ResponseType>
	class TwitterRESTServiceBase : public TFC::Net::OAuthRESTServiceBase<TClientInfoProvider, ResponseType>
	{
	public:
		template<ParameterType P, typename T>
		using Parameter = typename TFC::Net::RESTServiceBase<ResponseType>::template Parameter<P, T>;

		TwitterRESTServiceBase(std::string url, HTTPMode httpMode, std::string const& token, std::string const& tokenSecret);
	protected:
		virtual void OnBeforePrepareRequest();
	private:
		std::string tokenSecret;
		Parameter<TFC::Net::ParameterType::Query, std::string> Version;
		Parameter<TFC::Net::ParameterType::Query, std::string> SignatureMethod;
		Parameter<TFC::Net::ParameterType::Query, std::string> Signature;
		Parameter<TFC::Net::ParameterType::Query, std::string> ConsumerKey;
		Parameter<TFC::Net::ParameterType::Query, std::string> Token;
		Parameter<TFC::Net::ParameterType::Query, std::string> Nonce;
		Parameter<TFC::Net::ParameterType::Query, std::string> Timestamp;
	};
}
}

template<class ResponseType>
TFC::Net::OAuthRESTServiceTemplateBase<ResponseType>::OAuthRESTServiceTemplateBase(std::string url, HTTPMode httpMode, OAuthParam* param,
		std::string const& token, std::string const& oAuthCallback) :
		TFC::Net::RESTServiceBase<ResponseType>(url, httpMode),
		token(token),
		oAuthCallback(oAuthCallback),
		paramPtr(param),
		Authorization(this, "Authorization")
{
}

template<class ResponseType>
void TFC::Net::OAuthRESTServiceTemplateBase<ResponseType>::OnBeforePrepareRequest()
{
	AuthHeader authHeader(paramPtr->clientId, paramPtr->clientSecret, token, oAuthCallback);
	std::string base = CreateSignatureBaseString(this->Url, this->httpMode, this->postDataParam, this->queryStringParam, authHeader);
	CalculateSignature(base, authHeader);

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


template<typename TClientInfoProvider, class ResponseType>
TFC::Net::TwitterRESTServiceBase<TClientInfoProvider, ResponseType>::TwitterRESTServiceBase(
		std::string url, HTTPMode httpMode, std::string const& token, std::string const& tokenSecret) :
	OAuthRESTServiceBase<TClientInfoProvider, ResponseType>(url, httpMode, token),
	tokenSecret(tokenSecret),
	Version(this, "oauth_version"),
	SignatureMethod(this, "oauth_signature_method"),
	Signature(this, "oauth_signature"),
	ConsumerKey(this, "oauth_consumer_key"),
	Token(this, "oauth_token"),
	Nonce(this, "oauth_nonce"),
	Timestamp(this, "oauth_timestamp")
{

}

template<typename TClientInfoProvider, class ResponseType>
void TFC::Net::TwitterRESTServiceBase<TClientInfoProvider, ResponseType>::OnBeforePrepareRequest()
{
	AuthHeader authHeader(this->paramPtr->clientId, this->paramPtr->clientSecret, this->token, "");
	std::string base = CreateSignatureBaseString(this->Url, this->httpMode, this->postDataParam, this->queryStringParam, authHeader);
	std::string signature = CalculateSignature(base, authHeader, tokenSecret);

	Version = authHeader.headers["oauth_version"];
	SignatureMethod = authHeader.headers["oauth_signature_method"];
	Signature = signature;
	ConsumerKey = authHeader.headers["oauth_consumer_key"];
	Token = authHeader.headers["oauth_token"];
	Nonce = authHeader.headers["oauth_nonce"];
	Timestamp = authHeader.headers["oauth_timestamp"];
}

#endif /* TFC_NET_OAUTHREST_H_ */
