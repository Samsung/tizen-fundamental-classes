/*
 * OAuthTest.cpp
 *
 *  Created on: Nov 14, 2016
 *      Author: gilang
 */



#include "TFC/Net/OAuth.h"
#include "TFC/Net/REST.h"
#include "TFC_Test.h"

#include <gtest/gtest.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <fstream>

class OAuthTest : public testing::Test
{
	protected:
	// virtual void SetUp() will be called before each test is run.
	// You should define it if you need to initialize the variables.
	// Otherwise, you don't have to provide it.
	virtual void SetUp()
	{

	}
	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.
	// Otherwise, you don't have to provide it.
	virtual void TearDown()
	{

	}
};

namespace {
	struct FacebookOAuthProvider
	{
		static constexpr char const* authUrl = "https://www.facebook.com/dialog/oauth";
		static constexpr char const* refreshTokenUrl = "https://www.facebook.com/dialog/oauth";
	};

	struct FacebookAppClientProvider
	{
		static constexpr char const* clientId = "492256874254380";
		static constexpr char const* clientScope = "public_profile, email";
		static constexpr char const* redirectionUrl = "http://galaxygift.id/";
	};

	struct GoogleOAuthProvider
	{
		static constexpr char const* authUrl = "https://accounts.google.com/o/oauth2/auth";
		static constexpr char const* accessTokenUrl = "https://accounts.google.com/o/oauth2/token";
		static constexpr char const* refreshTokenUrl = "https://www.googleapis.com/oauth2/v3/token";
	};

	struct GoogleAppClientProvider
	{
		static constexpr char const* clientId = "216020663022-qo7o09pun912281apcal6bfvco0m7d64.apps.googleusercontent.com";
		static constexpr char const* clientScope = "email";
		static constexpr char const* clientSecret = "DEdQUn-QFbiitgpGyLd6dtoZ";
		static constexpr char const* redirectionUrl = "http://heliosky.com";
	};

	struct TwitterOAuthProvider
	{
		static constexpr char const* authUrl = "https://api.twitter.com/oauth/authenticate";
		static constexpr char const* accessTokenUrl = "https://api.twitter.com/oauth/access_token";
		static constexpr char const* requestTokenUrl = "https://api.twitter.com/oauth/request_token";
		static constexpr auto oAuthMode = TFC::Net::OAuthMode::Version1;
	};

	struct TwitterAppClientProvider
	{
		static constexpr char const* clientId = "vOKnOpVfvlYlPiGbZ2Qgtga1I";
		static constexpr char const* clientScope = "email";
		static constexpr char const* clientSecret = "dyNtTL8G71QTv1X41m4D6fb605m5vKh0hZ1KDV2NpcdQLx2ZbT";
		static constexpr char const* redirectionUrl = "http://galaxygift.id/";
	};

	struct InvalidOAuthProvider
	{
		static constexpr char const* invalidAuth = "http://invalid.org";
	};

	struct AnotherInvalidAuthProvider
	{
		static constexpr int authUrl = 123;
	};
}

TEST_F(OAuthTest, OAuthProviderTest)
{


}

TEST_F(OAuthTest, RetrieveAuthUrl)
{
	typedef TFC::Net::ServiceInfoExtractor<FacebookOAuthProvider> Extractor;

	auto res1 = Extractor::authUrl;
	auto res2 = Extractor::accessTokenUrl;
	auto res4 = Extractor::refreshTokenUrl;

	EXPECT_STREQ("https://www.facebook.com/dialog/oauth", res1) << "Metaprogramming failed to retrieve value";
	EXPECT_STREQ("https://www.facebook.com/dialog/oauth", res4) << "Metaprogramming failed to retrieve value";
	EXPECT_EQ(nullptr, res2) << "Metaprogramming failed to retrieve value";
}

class OAuthTestClass : public TFC::EventClass
{
public:
	typedef TFC::Net::OAuthClient<GoogleOAuthProvider, GoogleAppClientProvider> FBClient;
	FBClient client;
	std::timed_mutex mutex;
	std::string accessToken;

	void OnAccessTokenReceived(TFC::Net::OAuthClientBase* src, std::string token)
	{
		accessToken = token;
		std::cout << "Access Token : " << accessToken << "\n";
		mutex.unlock();
	}

	OAuthTestClass()
	{
		client.eventAccessTokenReceived += EventHandler(OAuthTestClass::OnAccessTokenReceived);
	}

	void PerformRequest()
	{
		mutex.lock();
		client.PerformRequest();
	}
};

class GoogleTokenVerify : public TFC::Net::RESTServiceBase<bool>
{
public:
	Parameter<TFC::Net::ParameterType::Query, std::string> AccessToken;

	GoogleTokenVerify() :
		RESTServiceBase("https://www.googleapis.com/oauth2/v1/tokeninfo", TFC::Net::HTTPMode::Get),
		AccessToken(this, "access_token")
	{
	}
protected:
	virtual bool* OnProcessResponse(int httpCode, const std::string& responseStr, int& errorCode, std::string& errorMessage)
	{
		return new bool(responseStr.find("invalid_token") == std::string::npos);
	}
};

TEST_F(OAuthTest, FullStackOAuth)
{
	OAuthTestClass tc;

	EFL_BLOCK_BEGIN;
		EFL_SYNC_BEGIN(tc);
			tc.PerformRequest();
		EFL_SYNC_END;
	EFL_BLOCK_END;

	tc.mutex.lock();

	EXPECT_LT(2, tc.accessToken.length()) << "OAuth token not received.";

	GoogleTokenVerify verify;
	verify.AccessToken = tc.accessToken;
	auto result = verify.Call();
	EXPECT_EQ(true, *(result.Response)) << "OAuth token is invalid.";
}



class TwitterOAuthTestClass : public TFC::EventClass
{
public:
	typedef TFC::Net::OAuthClient<TwitterOAuthProvider, TwitterAppClientProvider> TwitterClient;
	TwitterClient client;
	std::timed_mutex mutex;
	std::string accessToken;

	void OnAccessTokenReceived(TFC::Net::OAuthClientBase* src, TFC::Net::OAuthToken* tok)
	{
		accessToken = tok->token;
		std::cout << "Access Token : " << accessToken << "\n";
		std::cout << "Access Token Secret : " << tok->secret << "\n";
		delete tok;
		mutex.unlock();
	}

	TwitterOAuthTestClass()
	{
		client.eventAccessTokenSecretReceived += EventHandler(TwitterOAuthTestClass::OnAccessTokenReceived);
	}

	void PerformRequest()
	{
		mutex.lock();
		try
		{
		client.PerformRequest();
		}
		catch (TFC::Net::OAuth2Exception& ex)
		{
			std::cout << ex.what() << "\n";
		}
	}
};

TEST_F(OAuthTest, TwitterStackOAuth)
{
	TwitterOAuthTestClass tc;

	EFL_BLOCK_BEGIN;
		EFL_SYNC_BEGIN(tc);
			tc.PerformRequest();
		EFL_SYNC_END;
	EFL_BLOCK_END;

	tc.mutex.lock();

	EXPECT_LT(2, tc.accessToken.length()) << "OAuth token not received.";
}
