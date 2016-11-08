/*
 * RESTTest.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: Kevin Winata
 */

#include "TFC/Net/REST.h"
#include "TFC_Test.h"

#include <gtest/gtest.h>
#include <mutex>
#include <chrono>
#include <thread>

class RESTTest : public testing::Test
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

class GetService : public TFC::Net::RESTServiceBase<std::string>
{
public:
	GetService() :
		RESTServiceBase("https://www.google.com.vn/", TFC::Net::HTTPMode::Get)
	{
	}
protected:
	virtual std::string* OnProcessResponse(int httpCode, const std::string& responseStr, int& errorCode, std::string& errorMessage)
	{
		return new std::string(responseStr);
	}
};

TEST_F(RESTTest, RESTGetCallingTest)
{
	GetService gs;
	auto result = gs.Call();
	ASSERT_NE(0, result.Response->size());
}

class RESTAsyncTestClass : public TFC::EventClass
{

};

