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
 *    Net/OAuth.h
 *
 * Helper classes to perform OAuth in TFC applications
 *
 * Created on: 	Nov 14, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#ifndef TFC_NET_OAUTH_H_
#define TFC_NET_OAUTH_H_


extern "C" {
#include <oauth2.h>
}

#include <type_traits>
#include "TFC/Core.h"

namespace TFC {
namespace Net {

enum class OAuthMode {
	Version1,
	Version2
};

class OAuth2Exception : public TFCException
{

public:
	explicit OAuth2Exception(int oauthErrorCode);
	int GetErrorCode() const { return this->errorCode; }
private:
	int errorCode;
	static char const* GetErrorMessage(int code);
};



template<typename TServiceInfoProvider>
struct AuthUrlIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::authUrl), char const*>::type> static True  TestFunc(decltype(T::authUrl) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TServiceInfoProvider>(nullptr))) == sizeof(True) };
};


template<typename TServiceInfoProvider>
struct AccessTokenUrlIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::accessTokenUrl), char const*>::type> static True  TestFunc(decltype(T::accessTokenUrl) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TServiceInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TServiceInfoProvider>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::accessTokenUrl; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TServiceInfoProvider>
struct RequestTokenUrlIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::requestTokenUrl), char const*>::type> static True  TestFunc(decltype(T::requestTokenUrl) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TServiceInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TServiceInfoProvider>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::requestTokenUrl; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TServiceInfoProvider>
struct RedirectionUrlIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::redirectionUrl), char const*>::type> static True  TestFunc(decltype(T::redirectionUrl) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TServiceInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TServiceInfoProvider>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::redirectionUrl; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TServiceInfoProvider>
struct RefreshTokenUrlIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::refreshTokenUrl), char const*>::type> static True  TestFunc(decltype(T::refreshTokenUrl) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TServiceInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TServiceInfoProvider>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::refreshTokenUrl; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TServiceInfoProvider>
struct ThreeLeggedIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::threeLegged), bool>::type> static True  TestFunc(decltype(T::threeLegged) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TServiceInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TServiceInfoProvider>
	struct Extractor { static constexpr bool value = false; };

	template<typename T>
	struct Extractor<true, T> { static constexpr bool value = T::threeLegged; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TServiceInfoProvider>
struct OAuthModeIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::oAuthMode), OAuthMode>::type> static True  TestFunc(decltype(T::oAuthMode)* = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TServiceInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TServiceInfoProvider>
	struct Extractor { static constexpr OAuthMode value = OAuthMode::Version2; };

	template<typename T>
	struct Extractor<true, T> { static constexpr OAuthMode value = T::oAuthMode; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TServiceInfoProvider>
struct ServiceInfoExtractor
{
	static constexpr auto authUrl = TServiceInfoProvider::authUrl;
	static constexpr auto oAuthMode = OAuthModeIntrospector<TServiceInfoProvider>::value;
	static constexpr auto accessTokenUrl = AccessTokenUrlIntrospector<TServiceInfoProvider>::value;
	static constexpr auto requestTokenUrl = RequestTokenUrlIntrospector<TServiceInfoProvider>::value;
	static constexpr auto refreshTokenUrl = RefreshTokenUrlIntrospector<TServiceInfoProvider>::value;
};

template<typename TClientInfoProvider>
struct ClientIdIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::clientId), char const*>::type> static True  TestFunc(decltype(T::clientId) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TClientInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TClientInfoProvider>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::clientId; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TClientInfoProvider>
struct ClientSecretIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::clientSecret), char const*>::type> static True  TestFunc(decltype(T::clientSecret) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TClientInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TClientInfoProvider>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::clientSecret; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TClientInfoProvider>
struct ClientScopeIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::clientScope), char const*>::type> static True  TestFunc(decltype(T::clientScope) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TClientInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TClientInfoProvider>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::clientScope; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TClientInfoProvider>
struct ClientInfoExtractor
{
	static constexpr auto clientId = TClientInfoProvider::clientId;
	static constexpr auto clientSecret = ClientSecretIntrospector<TClientInfoProvider>::value;
	static constexpr auto clientScope = ClientScopeIntrospector<TClientInfoProvider>::value;
	static constexpr auto redirectionUrl = TClientInfoProvider::redirectionUrl;
};

struct OAuthParam
{
	// This is provided by ServiceInfo provider class
	char const* authUrl;
	char const* accessTokenUrl;
	char const* requestTokenUrl;
	char const* refreshTokenUrl;
	OAuthMode 	oAuthMode;

	// This is provided by ClientInfo provider class
	char const* clientId;
	char const* clientSecret;
	char const* clientScope;
	char const* redirectionUrl;


	template<typename TServiceInfoProvider, typename TClientInfoProvider>
	static OAuthParam* Build()
	{
		typedef ServiceInfoExtractor<TServiceInfoProvider> ServiceInfo;
		typedef ClientInfoExtractor<TClientInfoProvider> ClientInfo;

		return new OAuthParam({
			ServiceInfo::authUrl,
			ServiceInfo::accessTokenUrl,
			ServiceInfo::requestTokenUrl,
			ServiceInfo::refreshTokenUrl,
			ServiceInfo::oAuthMode,
			ClientInfo::clientId,
			ClientInfo::clientSecret,
			ClientInfo::clientScope,
			ClientInfo::redirectionUrl
		});
	}

	template<typename TClientInfoProvider>
	static OAuthParam* BuildClientInfo()
	{
		typedef ClientInfoExtractor<TClientInfoProvider> ClientInfo;

		return new OAuthParam({
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			OAuthMode::Version2,
			ClientInfo::clientId,
			ClientInfo::clientSecret,
			ClientInfo::clientScope,
			ClientInfo::redirectionUrl
		});
	}
};

struct OAuthToken
{
	std::string token;
	std::string secret;
	OAuthToken(std::string const& response);
};

struct OAuthGrant
{
	std::string token;
	std::string verifier;
	OAuthGrant(std::string const& url);
};

class OAuthWindow;

class OAuthClientBase : public EventEmitterClass<OAuthClientBase>
{
public:
	Event<std::string> eventAccessTokenReceived;
	Event<std::string> eventRefreshTokenReceived;
	Event<OAuthToken*> eventAccessTokenSecretReceived;

	void PerformRequest();
	void PerformXAuthRequest(std::string const& username, std::string const& password);
	void RefreshToken(std::string oldToken);

protected:
	OAuthClientBase(OAuthParam* param);
	~OAuthClientBase();

private:
	oauth2_request_h requestHandle;
	oauth2_manager_h managerHandle;
	OAuthParam*		 paramPtr;
	bool busy;

	void PerformOAuth1Request();

	void CleanUpRequest();

	static void RequestAuthorizationCallback(oauth2_response_h response, void* thisObj);

	OAuthWindow* window;

	void OnAuthGrantCallbackCaptured(OAuthWindow* source, OAuthGrant data);


public:
	bool IsBusy() const { return this->busy; }
};

template<typename TServiceInfoProvider, typename TClientInfoProvider>
class OAuthClient : public OAuthClientBase
{
public:
	OAuthClient() : OAuthClientBase(OAuthParam::Build<TServiceInfoProvider, TClientInfoProvider>())
	{

	}
};



}
}

#endif /* TFC_NET_OAUTH_H_ */
