/*
 * OAuth.h
 *
 *  Created on: Nov 14, 2016
 *      Author: gilang
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
struct ServiceInfoExtractor
{
	static constexpr auto authUrl = TServiceInfoProvider::authUrl;
	static constexpr auto accessTokenUrl = AccessTokenUrlIntrospector<TServiceInfoProvider>::value;
	static constexpr auto requestTokenUrl = RequestTokenUrlIntrospector<TServiceInfoProvider>::value;
	static constexpr auto refreshTokenUrl = RefreshTokenUrlIntrospector<TServiceInfoProvider>::value;
	static constexpr auto redirectionUrl = RedirectionUrlIntrospector<TServiceInfoProvider>::value;
	static constexpr bool threeLegged = ThreeLeggedIntrospector<TServiceInfoProvider>::value;
};

template<typename TUserInfoProvider>
struct ClientIdIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::clientId), char const*>::type> static True  TestFunc(decltype(T::clientId) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TUserInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TUserInfoProvider>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::clientId; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TUserInfoProvider>
struct ClientSecretIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::clientSecret), char const*>::type> static True  TestFunc(decltype(T::clientSecret) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TUserInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TUserInfoProvider>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::clientSecret; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TUserInfoProvider>
struct ClientScopeIntrospector
{
	typedef long 	True;
	typedef char	False;

	template<typename T, typename = typename std::is_same<decltype(T::clientScope), char const*>::type> static True  TestFunc(decltype(T::clientScope) = nullptr);
	template<typename T> static False TestFunc(...);

	enum { available = sizeof(decltype(TestFunc<TUserInfoProvider>(nullptr))) == sizeof(True) };

	template<bool = available, typename = TUserInfoProvider>
	struct Extractor { static constexpr char const* value = nullptr; };

	template<typename T>
	struct Extractor<true, T> { static constexpr char const* value = T::clientScope; };

	static constexpr auto value = Extractor<>::value;
};

template<typename TUserInfoProvider>
struct UserInfoExtractor
{
	static constexpr auto clientId = TUserInfoProvider::clientId;
	static constexpr auto clientSecret = ClientSecretIntrospector<TUserInfoProvider>::value;
	static constexpr auto clientScope = ClientScopeIntrospector<TUserInfoProvider>::value;
};

struct OAuthParam
{
	// This is provided by ServiceInfo provider class
	char const* authUrl;
	char const* accessTokenUrl;
	char const* requestTokenUrl;
	char const* redirectionUrl;
	char const* refreshTokenUrl;

	// This is provided by ClientInfo provider class
	char const* clientId;
	char const* clientSecret;
	char const* clientScope;
	bool threeLegged;

	template<typename TServiceInfoProvider, typename TClientInfoProvider>
	static OAuthParam* Build()
	{
		typedef ServiceInfoExtractor<TServiceInfoProvider> ServiceInfo;
		typedef UserInfoExtractor<TClientInfoProvider> ClientInfo;

		return new OAuthParam({
			ServiceInfo::authUrl,
			ServiceInfo::accessTokenUrl,
			ServiceInfo::requestTokenUrl,
			ServiceInfo::redirectionUrl,
			ServiceInfo::refreshTokenUrl,
			ClientInfo::clientId,
			ClientInfo::clientSecret,
			ClientInfo::clientScope,
			ServiceInfo::threeLegged
		});
	}

	template<typename TClientInfoProvider>
	static OAuthParam* BuildClientInfo()
	{
		typedef UserInfoExtractor<TClientInfoProvider> ClientInfo;

		return new OAuthParam({
			ClientInfo::clientId,
			ClientInfo::clientSecret,
			ClientInfo::clientScope
		});
	}
};

class OAuthWindow;

class OAuth2ClientBase : public EventEmitterClass<OAuth2ClientBase>
{
public:
	Event<std::string> eventAccessTokenReceived;
	Event<std::string> eventRefreshTokenReceived;

	void PerformRequest();
	void PerformXAuthRequest(std::string const& username, std::string const& password);
	void RefreshToken(std::string oldToken);

protected:
	OAuth2ClientBase(OAuthParam* param);
	~OAuth2ClientBase();

private:
	oauth2_request_h requestHandle;
	oauth2_manager_h managerHandle;
	OAuthParam*		 paramPtr;
	bool busy;

	void PerformOAuth1Request();

	void CleanUpRequest();

	static void RequestAuthorizationCallback(oauth2_response_h response, void* thisObj);

	OAuthWindow* window;




public:
	bool IsBusy() const { return this->busy; }
};

template<typename TServiceInfoProvider, typename TClientInfoProvider>
class OAuth2Client : public OAuth2ClientBase
{
public:
	OAuth2Client() : OAuth2ClientBase(OAuthParam::Build<TServiceInfoProvider, TClientInfoProvider>())
	{

	}
};



}
}

#endif /* TFC_NET_OAUTH_H_ */
