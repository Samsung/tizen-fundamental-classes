/*
 * OAuthTest.cpp
 *
 *  Created on: Nov 14, 2016
 *      Author: gilang
 */



#include "TFC/Net/OAuth.h"
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
	auto res2 = Extractor::tokenUrl;
	auto res3 = Extractor::redirectionUrl;
	auto res4 = Extractor::refreshTokenUrl;

	EXPECT_STREQ("https://www.facebook.com/dialog/oauth", res1) << "Metaprogramming failed to retrieve value";
	EXPECT_STREQ("https://www.facebook.com/dialog/oauth", res4) << "Metaprogramming failed to retrieve value";
	EXPECT_EQ(nullptr, res2) << "Metaprogramming failed to retrieve value";
	EXPECT_EQ(nullptr, res3) << "Metaprogramming failed to retrieve value";
}

TEST_F(OAuthTest, FullStackOAuth)
{
	typedef TFC::Net::OAuth2Client<FacebookOAuthProvider, FacebookAppClientProvider> FBClient;
	FBClient client;
}
