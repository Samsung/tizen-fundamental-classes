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

struct OAuthParam
{
	// This is provided by ServiceInfo provider class
	char const* authUrl;
	char const* tokenUrl;
	char const* redirectionUrl;
	char const* refreshTokenUrl;

	// This is provided by ClientInfo provider class
	char const* clientId;
	char const* clientSecret;
	char const* clientScope;
	bool threeLegged;
};

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

	void CleanUpRequest();

	static void RequestAuthorizationCallback(oauth2_response_h response, void* thisObj);

public:
	bool IsBusy() const { return this->busy; }
};

template<typename TServiceInfoProvider, typename TClientInfoProvider>
class OAuth2Client
{

};



}
}

#endif /* TFC_NET_OAUTH_H_ */
