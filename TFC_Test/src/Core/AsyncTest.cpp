/*
 * AsyncTest.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: Gilang M. Hamidy
 */

#include "TFC/Async.new.h"

#include <gtest/gtest.h>

class AsyncTest : public testing::Test
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

TEST_F(AsyncTest, OperatorRedirectionTest)
{
	int a = 5;

	TFC::Core::SharedEventObject<TFC::Core::Async::AsyncTask<void>*, void*> eventAsync;
	auto operand = [a] () { int b = a; } >> eventAsync;

	/* This is invalid operation because the lambda has parameter
	auto operandInvalid = [a] (int b) { int c = b + a; } >> eventAsync;
	*/

	typedef TFC::Core::Async::AsyncOperand<void, decltype(eventAsync)> CorrectType;

	ASSERT_TRUE((std::is_same<decltype(operand), CorrectType>::value)) << "Operator << yields incorrect type";
}

TEST_F(AsyncTest, OperatorRedirectionTestNonVoid)
{
	TFC::Core::SharedEventObject<TFC::Core::Async::AsyncTask<std::string>*, std::string> eventAsync;
	auto operand = [] { return std::string("Hello!"); } >> eventAsync;

	typedef TFC::Core::Async::AsyncOperand<std::string, decltype(eventAsync)> CorrectType;
	ASSERT_TRUE((std::is_same<decltype(operand), CorrectType>::value)) << "Operator << yields incorrect type on non void";
}

TEST_F(AsyncTest, LambdaToAsyncOperand)
{
	TFC::Core::Async::AsyncOperand<void> operand([] { int a = 12 + 15; });

	auto tmp = TFC::Core::Async::AsyncBuilder() & [] { return 5; };
}
