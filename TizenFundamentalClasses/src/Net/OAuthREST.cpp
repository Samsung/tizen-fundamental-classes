/*
 * OAuthREST.cpp
 *
 *  Created on: Nov 29, 2016
 *      Author: gilang
 */

#include "TFC/Net/OAuthREST.h"
#include "TFC/Net/Util.h"
#include <chrono>
#include <algorithm>
#include <openssl/hmac.h>

using namespace TFC::Net;

std::string GenerateNonce(std::size_t length)
{
    std::string str(length, 0);
    std::generate_n(str.begin(), length, [] () -> char {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    });
    return str;
}

std::string GenerateTimestamp()
{
	auto now = std::chrono::system_clock::now();
	auto value = std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch();
	unsigned long duration = value.count();

	return std::to_string(duration);
}

std::string PercentEncode(std::string const& base)
{
	std::stringstream st;
	size_t len = base.length();
	auto str = base.c_str();

	for(int i = 0; i < len; i++)
	{
		if(isalnum(str[i]) || str[i] == '-' || str[i] == '.' || str[i] == '_' || str[i] == '~')
			st << str[i];
		else
			st << "%" << std::hex << std::uppercase << ((int)str[i]);
	}
	return st.str();
}

LIBAPI TFC::Net::AuthHeader::AuthHeader(
		std::string const& consumerKey,
		std::string const& consumerSecret,
		std::string const& token,
		std::string const& oAuthCallback) : consumerSecret(consumerSecret)
{
	headers["oauth_version"] = "1.0";
	headers["oauth_signature_method"] = "HMAC-SHA1";
	headers["oauth_signature"] = "";

	headers["oauth_consumer_key"] = consumerKey;
	headers["oauth_token"] = token;
	headers["oauth_callback"] = oAuthCallback;

	headers["oauth_nonce"] = GenerateNonce(32);
	headers["oauth_timestamp"] = GenerateTimestamp();
}

LIBAPI std::string TFC::Net::CreateSignatureBaseString(
		std::string const& url, HTTPMode httpMode,
		std::unordered_map<std::string, IServiceParameter*> const& postDataParam,
		std::vector<std::pair<char const*, IServiceParameter*>> const& queryStringParam,
		AuthHeader const& authHeader)
{
	std::map<std::string, std::string> paramMap;
	for (auto& kv : queryStringParam)
	{
		if (kv.second->isSet) {
			paramMap[kv.first] = kv.second->GetEncodedValue();
		}
	}
	for (auto& kv : postDataParam)
	{
		if (kv.second->isSet) {
			paramMap[kv.first] = kv.second->GetEncodedValue();
		}
	}
	for (auto& kv : authHeader.headers)
	{
		if (!kv.second.empty())
			paramMap.insert(kv);
	}

	std::stringstream paramStream;
	bool first = true;
	for (auto& kv : paramMap)
	{
		if (first) first = false;
		else paramStream << "&";

		paramStream << kv.first << "=" << kv.second;
	}

	std::stringstream resultStream;
	switch (httpMode) {
	case HTTPMode::Post :
		resultStream << "POST"; break;
	case HTTPMode::Delete :
		resultStream << "DELETE"; break;
	case HTTPMode::Get :
	default:
		resultStream << "GET";
	}
	resultStream << "&";
	resultStream << PercentEncode(url) << "&";
	resultStream << PercentEncode(paramStream.str());

	//std::cout << "Base signature : " << resultStream.str() << "\n";
	return resultStream.str();
}

LIBAPI std::string TFC::Net::CalculateSignature(std::string const& base, AuthHeader& authHeader)
{
	std::stringstream signingKey;
	signingKey << PercentEncode(authHeader.consumerSecret) << "&";
	signingKey << PercentEncode(authHeader.headers.at("oauth_token"));
	std::string key = signingKey.str();

	unsigned char* digest;
	digest = HMAC(EVP_sha1(), key.c_str(), key.length(), (unsigned char*)base.c_str(), base.length(), NULL, NULL);

	return authHeader.headers["oauth_signature"] = PercentEncode(TFC::Net::Base64Encode(digest, 20));
}

