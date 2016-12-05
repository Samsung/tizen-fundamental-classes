/*
 * OAuth.cpp
 *
 *  Created on: Nov 14, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Net/OAuth.h"
#include "TFC/Net/OAuthREST.h"
#include "TFC/EFL.h"
#include <iostream>

#define OAUTH_DECLARE_ERROR_MSG(ERR_CODE, ERR_STR) \
	template<> char const* TFC::Net::OAuth2ErrorSelector< ERR_CODE >::ErrorMessage = "OAuth2 Error: " ERR_STR

#define OAUTH_OP_CHECK_INIT int result
#define OAUTH_OP_CHECK_THROW(STATEMENT) result = STATEMENT; if (result != OAUTH2_ERROR_NONE) throw OAuth2Exception(result)

class TFC::Net::OAuthWindow : public TFC::EventEmitterClass<TFC::Net::OAuthWindow>,
							  public TFC::EFL::EFLProxyClass
{
public:
	OAuthWindow();

	void Show(std::string const& url, std::string const& expectedCallbackUrl);
	Event<std::string> eventCallbackCaptured;
private:
	std::string expectedCallbackUrl;

	EvasSmartEventTyped<char const*> eventUrlChanged;
	EvasSmartEvent eventLoadFinished;

	Evas_Object* ewk;
	Evas_Object* loadingPopup;
	Elm_Win* window;

	void ShowLoadingPopup();
	void HideLoadingPopup();

	void OnUrlChanged(Evas_Object* obj, char const* url);
	void OnLoadFinished(Evas_Object* obj, void* nop);
};

struct OAuthToken {
	std::string token;
	std::string secret;

	void ParseTokenSecret(std::string const& str)
	{
		auto tokenStart = str.find("oauth_token=") + 12;
		auto tokenEnd = str.find("&", tokenStart);
		auto secretStart = str.find("oauth_token_secret=") + 19;
		auto secretEnd = str.find("&", secretStart);

		token = str.substr(tokenStart, tokenEnd - tokenStart);
		secret = str.substr(secretStart, secretEnd - secretStart);
	}
};

class OAuth1TokenService : public TFC::Net::OAuthRESTServiceTemplateBase<OAuthToken>
{
public:
	OAuth1TokenService(std::string const& tokenUrl, TFC::Net::OAuthParam* param, std::string const& token, std::string const& oAuthCallback) :
		TFC::Net::OAuthRESTServiceTemplateBase<OAuthToken>(tokenUrl, TFC::Net::HTTPMode::Get, param, token, oAuthCallback)
	{
	}
protected:
	virtual OAuthToken* OnProcessResponse(int httpCode, const std::string& responseStr, int& errorCode, std::string& errorMessage)
	{
		std::cout << "\nRequest token response : \n" << responseStr << "\n";
		auto oAuthToken = new OAuthToken();
		oAuthToken->ParseTokenSecret(responseStr);
		return oAuthToken;
	}
};

TFC ::Net::OAuth2Exception::OAuth2Exception(int oauthErrorCode) :
		TFCException(GetErrorMessage(oauthErrorCode)),
		errorCode(oauthErrorCode)
{
}

const char* TFC::Net::OAuth2Exception::GetErrorMessage(int code)
{
	switch(code)
	{
	case OAUTH2_ERROR_NONE: 				return "None";
	case OAUTH2_ERROR_OUT_OF_MEMORY: 		return "Out of memory";
	case OAUTH2_ERROR_INVALID_PARAMETER: 	return "Invalid parameter";
	case OAUTH2_ERROR_ALREADY_IN_PROGRESS: 	return "Already in progress";
	case OAUTH2_ERROR_NOT_SUPPORTED: 		return "Not supported";
	case OAUTH2_ERROR_PERMISSION_DENIED: 	return "Permission denied";
	case OAUTH2_ERROR_NETWORK_ERROR: 		return "Network error";
	case OAUTH2_ERROR_SERVER: 				return "Server error";
	case OAUTH2_ERROR_USER_CANCELLED: 		return "User cancelled";
	case OAUTH2_ERROR_VALUE_NOT_FOUND: 		return "Value not found";
	case OAUTH2_ERROR_UNKNOWN: 				return "Unknown error";
	}
	return "";
}

LIBAPI
void TFC::Net::OAuth2ClientBase::PerformRequest()
{
	if (paramPtr == nullptr) return;

	if (paramPtr->threeLegged)
	{
		PerformOAuth1Request();
	}
	else
	{
		PerformXAuthRequest("", "");
	}
}

void TFC::Net::OAuth2ClientBase::PerformOAuth1Request()
{
	OAuth1TokenService service(paramPtr->requestTokenUrl, paramPtr, "", paramPtr->redirectionUrl);
	auto result = service.Call();
	OAuthToken* response = result.Response;
	std::cout << "Token : " << response->token << ", Secret : " << response->secret << "\n";
	eventAccessTokenReceived(this, response->token);
}

LIBAPI
void TFC::Net::OAuth2ClientBase::PerformXAuthRequest(
		const std::string& username, const std::string& password)
{
	OAUTH_OP_CHECK_INIT;

	OAUTH_OP_CHECK_THROW(oauth2_request_set_auth_end_point_url(requestHandle, paramPtr->authUrl));
	if (paramPtr->accessTokenUrl)
	{
		OAUTH_OP_CHECK_THROW(oauth2_request_set_token_end_point_url(requestHandle, paramPtr->accessTokenUrl));
	}
	OAUTH_OP_CHECK_THROW(oauth2_request_set_redirection_url(requestHandle, paramPtr->redirectionUrl));

	OAUTH_OP_CHECK_THROW(oauth2_request_set_client_id(requestHandle, paramPtr->clientId));
	if (paramPtr->clientSecret)
	{
		OAUTH_OP_CHECK_THROW(oauth2_request_set_client_secret(requestHandle, paramPtr->clientSecret));
	}
	OAUTH_OP_CHECK_THROW(oauth2_request_set_scope(requestHandle, paramPtr->clientScope));
	OAUTH_OP_CHECK_THROW(oauth2_request_set_response_type(requestHandle, OAUTH2_RESPONSE_TYPE_CODE));

	if (username.length() > 0 && password.length() > 0)
	{
		OAUTH_OP_CHECK_THROW(oauth2_request_set_user_name(requestHandle, username.c_str()));
		OAUTH_OP_CHECK_THROW(oauth2_request_set_password(requestHandle, password.c_str()));
	}
	OAUTH_OP_CHECK_THROW(oauth2_manager_request_token(managerHandle, requestHandle, &RequestAuthorizationCallback, this));
}

LIBAPI
void TFC::Net::OAuth2ClientBase::RefreshToken(std::string oldToken) {
}

LIBAPI
TFC::Net::OAuth2ClientBase::OAuth2ClientBase(OAuthParam* param) :
		paramPtr(param),
		busy(false),
		window(nullptr)
{
	if (!paramPtr->threeLegged)
	{
		OAUTH_OP_CHECK_INIT;
		OAUTH_OP_CHECK_THROW(oauth2_manager_create(&this->managerHandle));
		OAUTH_OP_CHECK_THROW(oauth2_request_create(&this->requestHandle));
	}
}

LIBAPI
TFC::Net::OAuth2ClientBase::~OAuth2ClientBase()
{
	if (!paramPtr->threeLegged)
	{
		CleanUpRequest();

		// Clean up manager
		if(this->managerHandle != nullptr)
		{
			oauth2_manager_destroy(this->managerHandle);
			this->managerHandle = nullptr;
		}

		// Clean up param
		if(this->paramPtr != nullptr)
		{
			delete this->paramPtr;
			this->paramPtr = nullptr;
		}
	}

	if(window != nullptr)
		delete window;
}

void TFC::Net::OAuth2ClientBase::CleanUpRequest() {
	if(this->requestHandle != nullptr)
	{
		oauth2_request_destroy(this->requestHandle);
		this->requestHandle = nullptr;
	}
}

void TFC::Net::OAuth2ClientBase::RequestAuthorizationCallback(
		oauth2_response_h response, void* thisObj)
{
	oauth2_error_h err;
	int serverErrorCode = 0, platformErrorCode = 0;
	oauth2_response_get_error(response, &err);
	oauth2_error_get_code(err, &serverErrorCode, &platformErrorCode);

	if(serverErrorCode != 0 || platformErrorCode != 0)
	{
		char* errorDescription;
		oauth2_error_get_description(err, &errorDescription);
		std::cout << "Error : " << errorDescription << "\n";
	}

	char* token;
	oauth2_response_get_access_token(response, &token);

	auto oAuth2ClientPtr = static_cast<OAuth2ClientBase*>(thisObj);
	oAuth2ClientPtr->eventAccessTokenReceived(oAuth2ClientPtr, token);
}

//// IMPLEMENTATION FOR OAUTHWINDOW

TFC::Net::OAuthWindow::OAuthWindow()
{
}

void TFC::Net::OAuthWindow::Show(const std::string& url, const std::string& expectedCallbackUrl)
{
}

void TFC::Net::OAuthWindow::ShowLoadingPopup()
{
}

void TFC::Net::OAuthWindow::HideLoadingPopup()
{
}

void TFC::Net::OAuthWindow::OnUrlChanged(Evas_Object* obj, const char* url)
{
}

void TFC::Net::OAuthWindow::OnLoadFinished(Evas_Object* obj, void* nop)
{
}
