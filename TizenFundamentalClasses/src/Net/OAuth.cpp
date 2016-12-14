/*
 * OAuth.cpp
 *
 *  Created on: Nov 14, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Net/OAuth.h"
#include "TFC/Net/OAuthREST.h"
#include "TFC/EFL.h"
#include "TFC/Async.h"

#include <iostream>
#include <dlog.h>
#include <efl_extension.h>
#include <EWebKit.h>
#include <system_info.h>

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
	void Hide();
	Event<OAuthGrant> eventCallbackCaptured;
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

TFC::Net::OAuthToken::OAuthToken(std::string const& str)
{
	auto tokenStart = str.find("oauth_token=") + 12;
	auto tokenEnd = str.find("&", tokenStart);
	auto secretStart = str.find("oauth_token_secret=") + 19;
	auto secretEnd = str.find("&", secretStart);

	if (tokenEnd != std::string::npos && secretEnd != std::string::npos)
	{
		token = str.substr(tokenStart, tokenEnd - tokenStart);
		secret = str.substr(secretStart, secretEnd - secretStart);
	}
};

TFC::Net::OAuthGrant::OAuthGrant(std::string const& str)
{
	auto tokenStart = str.find("oauth_token=") + 12;
	auto tokenEnd = str.find("&", tokenStart);
	auto verifierStart = str.find("oauth_verifier=") + 15;

	if (tokenEnd != std::string::npos)
	{
		token = str.substr(tokenStart, tokenEnd - tokenStart);
		verifier = str.substr(verifierStart, str.length() - verifierStart);
	}
	//std::cout << "Token : " << token << ", Verifier : " << verifier << "\n";
}

class OAuth1TokenService : public TFC::Net::OAuthRESTServiceTemplateBase<TFC::Net::OAuthToken>
{
public:
	Parameter<TFC::Net::ParameterType::PostData, std::string> Verifier;

	OAuth1TokenService(std::string const& tokenUrl, TFC::Net::OAuthParam* param, std::string const& token, std::string const& oAuthCallback) :
		TFC::Net::OAuthRESTServiceTemplateBase<TFC::Net::OAuthToken>(tokenUrl, TFC::Net::HTTPMode::Post, param, token, oAuthCallback),
		Verifier(this, "oauth_verifier")
	{
	}
protected:
	virtual TFC::Net::OAuthToken* OnProcessResponse(int httpCode, const std::string& responseStr, int& errorCode, std::string& errorMessage)
	{
		//std::cout << "OAuth Client Response : " << responseStr << "\n";
		dlog_print(DLOG_DEBUG, LOG_TAG, "OAuth Client Response : %s", responseStr.c_str());
		auto oAuthToken = new TFC::Net::OAuthToken(responseStr);
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
void TFC::Net::OAuthClientBase::PerformRequest()
{
	if (paramPtr == nullptr) return;

	if (paramPtr->oAuthMode == OAuthMode::Version1)
	{
		PerformOAuth1Request();
	}
	else
	{
		PerformXAuthRequest("", "");
	}
}

void TFC::Net::OAuthClientBase::PerformOAuth1Request()
{
	window = new OAuthWindow();
	window->eventCallbackCaptured += EventHandler(OAuthClientBase::OnAuthGrantCallbackCaptured);
	tfc_async
	{
		OAuth1TokenService service(paramPtr->requestTokenUrl, paramPtr, "", paramPtr->redirectionUrl);
		auto result = service.Call();
		return (OAuthToken*)result.Response;
	}
	tfc_async_complete (OAuthToken* response)
	{
		if (response == nullptr) throw OAuth2Exception(OAUTH2_ERROR_NETWORK_ERROR);

		//std::cout << "Request Token : " << response->token << ", Secret : " << response->secret << "\n";
		dlog_print(DLOG_DEBUG, LOG_TAG, "Request token : %s, secret : %s", response->token.c_str(), response->secret.c_str());
		std::stringstream urlStream;
		urlStream << paramPtr->authUrl << "?oauth_token=" << response->token;
		window->Show(urlStream.str(), paramPtr->redirectionUrl);
	};
}

void TFC::Net::OAuthClientBase::OnAuthGrantCallbackCaptured(OAuthWindow* source, OAuthGrant data)
{
	tfc_async
	{
		OAuth1TokenService service(paramPtr->accessTokenUrl, paramPtr, data.token, "");
		service.Verifier = data.verifier;
		auto result = service.Call();
		return (OAuthToken*)result.Response;
	}
	tfc_async_complete (OAuthToken* response)
	{
		if (response == nullptr) throw OAuth2Exception(OAUTH2_ERROR_NETWORK_ERROR);

		//std::cout << "Access Token : " << response->token << ", Secret : " << response->secret << "\n";
		dlog_print(DLOG_DEBUG, LOG_TAG, "Access token : %s, secret : %s", response->token.c_str(), response->secret.c_str());
		eventAccessTokenReceived(this, response->token);
		window->Hide();
	};
}

LIBAPI
void TFC::Net::OAuthClientBase::PerformXAuthRequest(
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
void TFC::Net::OAuthClientBase::RefreshToken(std::string oldToken) {
}

LIBAPI
TFC::Net::OAuthClientBase::OAuthClientBase(OAuthParam* param) :
		paramPtr(param),
		busy(false),
		window(nullptr)
{
	if (paramPtr->oAuthMode != OAuthMode::Version1)
	{
		OAUTH_OP_CHECK_INIT;
		OAUTH_OP_CHECK_THROW(oauth2_manager_create(&this->managerHandle));
		OAUTH_OP_CHECK_THROW(oauth2_request_create(&this->requestHandle));
	}
	else
	{
	}
}

LIBAPI
TFC::Net::OAuthClientBase::~OAuthClientBase()
{
	if (paramPtr->oAuthMode != OAuthMode::Version1)
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

void TFC::Net::OAuthClientBase::CleanUpRequest() {
	if(this->requestHandle != nullptr)
	{
		oauth2_request_destroy(this->requestHandle);
		this->requestHandle = nullptr;
	}
}

void TFC::Net::OAuthClientBase::RequestAuthorizationCallback(
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
		//std::cout << "Error : " << errorDescription << "\n";
		dlog_print(DLOG_DEBUG, LOG_TAG, "OAuth Client Error : %s", errorDescription);
	}

	char* token;
	oauth2_response_get_access_token(response, &token);

	auto oAuth2ClientPtr = static_cast<OAuthClientBase*>(thisObj);
	oAuth2ClientPtr->eventAccessTokenReceived(oAuth2ClientPtr, token);
}

//// IMPLEMENTATION FOR OAUTHWINDOW

TFC::Net::OAuthWindow::OAuthWindow() :
		ewk(nullptr),
		loadingPopup(nullptr),
		window(nullptr)
{
	ewk_init();

	eventLoadFinished += EventHandler(OAuthWindow::OnLoadFinished);
	eventUrlChanged += EventHandler(OAuthWindow::OnUrlChanged);

	window = elm_win_util_standard_add("Login", "");
	eext_object_event_callback_add(window, EEXT_CALLBACK_BACK, [] (void *data, Evas_Object *p, void *info) {
		auto thiz = static_cast<OAuthWindow*>(data);
		thiz->Hide();
	}, this);
	evas_object_show(window);

	ShowLoadingPopup();
}

void TFC::Net::OAuthWindow::Show(const std::string& url, const std::string& expectedCallbackUrl)
{
	HideLoadingPopup();
	this->expectedCallbackUrl = expectedCallbackUrl;

	auto box = elm_box_add(window);
	elm_box_padding_set(box, 0, 3);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(box, 0.0, 0.0);
	evas_object_show(box);

	auto canvas = evas_object_evas_get(window);

	ewk = ewk_view_add(canvas);
	if (ewk == nullptr)
		return;

	int w = 0, h = 0;
	system_info_get_platform_int("tizen.org/feature/screen.width", &w);
	system_info_get_platform_int("tizen.org/feature/screen.height", &h);
	ewk_view_url_set(ewk, url.c_str());
	evas_object_size_hint_min_set(ewk, w, h);

	evas_object_size_hint_weight_set(ewk, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ewk, EVAS_HINT_FILL, EVAS_HINT_FILL);

	eventUrlChanged.Bind(ewk, "url,changed");
	eventLoadFinished.Bind(ewk, "load,finished");

	elm_box_pack_end(box, ewk);
	evas_object_show(ewk);

	evas_object_show(window);
}

void TFC::Net::OAuthWindow::Hide()
{
	if (ewk)
	{
		ewk_view_stop(ewk);
		evas_object_hide(ewk);
		evas_object_hide(window);
	}
	HideLoadingPopup();
	ewk = nullptr;
	window = nullptr;
	loadingPopup = nullptr;
}

void TFC::Net::OAuthWindow::ShowLoadingPopup()
{
	loadingPopup = elm_popup_add(window);
	elm_popup_content_text_wrap_type_set(loadingPopup, ELM_WRAP_MIXED);
	elm_object_text_set(loadingPopup, "Loading...");
	elm_popup_orient_set(loadingPopup, ELM_POPUP_ORIENT_BOTTOM);
	evas_object_show(loadingPopup);
}

void TFC::Net::OAuthWindow::HideLoadingPopup()
{
	if (loadingPopup)
	{
		evas_object_hide(loadingPopup);
	}
}

void TFC::Net::OAuthWindow::OnUrlChanged(Evas_Object* obj, const char* url)
{
	std::string urlStr(url);
	//std::cout << "URL change : " << urlStr << "\n";
	if (urlStr.find(expectedCallbackUrl) != std::string::npos)
	{
		ShowLoadingPopup();
		//std::cout << "Expected URL with token : " << urlStr << "\n\n";
		OAuthGrant grant(urlStr);
		eventCallbackCaptured(this, grant);
	}
}

void TFC::Net::OAuthWindow::OnLoadFinished(Evas_Object* obj, void* nop)
{
	HideLoadingPopup();
}
