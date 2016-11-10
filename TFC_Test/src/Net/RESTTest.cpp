/*
 * RESTTest.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: Kevin Winata
 */

#include "TFC/Net/REST.h"
#include "TFC/Async.h"
#include "TFC_Test.h"

#include <gtest/gtest.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <fstream>

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

void readFile(std::stringstream& buffer, char const* path)
{
	std::string respath(app_get_resource_path());
	std::ifstream in(respath + path);
	buffer << in.rdbuf();
	in.close();
}

class GetService : public TFC::Net::RESTServiceBase<std::string>
{
public:
	Parameter<TFC::Net::ParameterType::Query, int> UserId;

	GetService() :
		RESTServiceBase("https://jsonplaceholder.typicode.com/posts", TFC::Net::HTTPMode::Get),
		UserId(this, "userId")
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
	std::string* response = result.Response;
	ASSERT_NE(0, response->size());

	std::stringstream buffer;
	readFile(buffer, "testcase/REST/get_posts_all.txt");
	EXPECT_EQ(buffer.str(), *response);

	delete response;
}

TEST_F(RESTTest, RESTGetWithParameterTest)
{
	GetService gs;
	gs.UserId = 1;

	auto result = gs.Call();
	std::string* response = result.Response;
	ASSERT_NE(0, response->size());

	std::stringstream buffer;
	readFile(buffer, "testcase/REST/get_posts_userid-1.txt");
	EXPECT_EQ(buffer.str(), *response);

	delete response;
}

class PostService : public TFC::Net::RESTServiceBase<std::string>
{
public:
	Parameter<TFC::Net::ParameterType::PostData, int> UserId;
	Parameter<TFC::Net::ParameterType::PostData, std::string> Title;
	Parameter<TFC::Net::ParameterType::PostData, std::string> Body;

	PostService() :
		RESTServiceBase("https://jsonplaceholder.typicode.com/posts", TFC::Net::HTTPMode::Post),
		UserId(this, "userId"),
		Title(this, "title"),
		Body(this, "body")
	{
	}
protected:
	virtual std::string* OnProcessResponse(int httpCode, const std::string& responseStr, int& errorCode, std::string& errorMessage)
	{
		return new std::string(responseStr);
	}
};

TEST_F(RESTTest, RESTPostCallingTest)
{
	PostService ps;
	ps.UserId = 1;
	ps.Title = "Dummy Title";
	ps.Body = "Lorem ipsum dolor sit amet.";

	auto result = ps.Call();
	std::string* response = result.Response;
	ASSERT_NE(0, response->size());

	EXPECT_EQ("{\n  \"body\": \"Lorem ipsum dolor sit amet.\",\n  \"title\": \"Dummy Title\",\n  \"userId\": 1,\n  \"id\": 101\n}", *response);

	delete response;
}

class RESTAsyncTestClass : public TFC::EventClass
{
public:
	std::timed_mutex mutexAsync;
	std::string result;

	RESTAsyncTestClass()
	{
	}

	void StartAsync()
	{
		mutexAsync.lock();
		std::shared_ptr<GetService> gs;
		gs->UserId = 1;
		tfc_async {
			return gs->Call();
		}
		tfc_async_complete(TFC::Net::RESTResult<std::string> res) {
			std::string* response = res.Response;
			result = *response;
			mutexAsync.unlock();
			delete response;
		};
	}
};

TEST_F(RESTTest, RESTAsyncTest)
{
	using Ms = std::chrono::milliseconds;
	RESTAsyncTestClass tc;

	EFL_BLOCK_BEGIN;
		EFL_SYNC_BEGIN(tc);

			tc.StartAsync();

		EFL_SYNC_END;
	EFL_BLOCK_END;

	bool success = tc.mutexAsync.try_lock_for(Ms(20000));
	ASSERT_TRUE(success) << "Asynchronous wait timeout";

	if(success)
	{
		std::stringstream buffer;
		readFile(buffer, "testcase/REST/get_posts_userid-1.txt");
		EXPECT_EQ(buffer.str(), tc.result);
	}
}
