/*
 * AsyncTest.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: Gilang M. Hamidy
 */

#include "TFC/Async.h"
#include "TFC_Test.h"

#include <gtest/gtest.h>
#include <mutex>
#include <chrono>
#include <thread>

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

class AsyncTestClass : public TFC::EventClass
{
public:
	int testCase;
	int result;
	TFC::Async<int>::Event eventAsyncCompleted;
	std::timed_mutex mutexAsync;

	void OnAsyncCompleted(decltype(eventAsyncCompleted)::Type* ev, TFC::Async<int>::Task* task, int ret)
	{
		result = ret;
		mutexAsync.unlock();
	}

	AsyncTestClass()
	{
		result = 0;
		testCase = rand();
		eventAsyncCompleted += EventHandler(AsyncTestClass::OnAsyncCompleted);
	}

	void StartAsync()
	{
		mutexAsync.lock();

		tfc_async {
			return this->testCase;
		} >> eventAsyncCompleted;
	}
};

TEST_F(AsyncTest, CompleteAsync)
{
	using namespace TFC;
	using Ms = std::chrono::milliseconds;
	AsyncTestClass tc;

	EFL_BLOCK_BEGIN;
		EFL_SYNC_BEGIN(tc);

			tc.StartAsync();

		EFL_SYNC_END;
	EFL_BLOCK_END;

	bool success = tc.mutexAsync.try_lock_for(Ms(1000));

	ASSERT_TRUE(success) << "Asynchronous wait timeout";

	if(success)
	{
		EXPECT_EQ(tc.testCase, tc.result) << "Asynchronous completed but result is wrong";
	}
}

TEST_F(AsyncTest, AsyncWithAwait)
{
	using namespace TFC;
	using Ms = std::chrono::milliseconds;

	int tc1 = rand();
	int result = 0;

	EFL_BLOCK_BEGIN;
		struct {
			int tc1;
			int& result;
		} data = {
			tc1,
			result
		};

		EFL_SYNC_BEGIN(data);

			int tmp = data.tc1;

			auto task = tfc_async {
				std::this_thread::sleep_for(Ms(2000));
				return tmp + 12345;
			};

			data.result = tfc_await task;

			std::cout << "Result here: " << data.result << std::endl;

		EFL_SYNC_END;
	EFL_BLOCK_END;

	EXPECT_EQ(tc1 + 12345, result) << "Asynchronous completed but result is wrong";
}

TEST_F(AsyncTest, AsyncWithFinally)
{
	using namespace TFC;
	using Ms = std::chrono::milliseconds;
	std::mutex mutexTest;
	mutexTest.lock();
	bool correct = false;

	EFL_BLOCK_BEGIN;
		struct {
			bool& correct;
			int tc;
			std::mutex& mutexTest;
		} data = {
			correct,
			rand(),
			mutexTest
		};

		EFL_SYNC_BEGIN(data);
			tfc_async
			{
				return data.tc + 54321;
			}
			tfc_async_complete(int result)
			{
				data.correct = result == (data.tc + 54321);
				data.mutexTest.unlock();
			};
		EFL_SYNC_END;
	EFL_BLOCK_END;

	mutexTest.lock();
	ASSERT_TRUE(correct) << "Asynchronous operation with finally failed";

}

TEST_F(AsyncTest, AsyncWithFinallyVoid)
{
	using namespace TFC;
	using Ms = std::chrono::milliseconds;
	std::mutex mutexTest;
	mutexTest.lock();
	bool correct = false;

	EFL_BLOCK_BEGIN;
		struct {
			bool& correct;
			int tc;
			std::mutex& mutexTest;
		} data = {
			correct,
			rand(),
			mutexTest
		};

		EFL_SYNC_BEGIN(data);
			tfc_async
			{
				std::this_thread::sleep_for(Ms(2000));
			}
			tfc_async_complete
			{
				data.correct = true;
				data.mutexTest.unlock();
			};
		EFL_SYNC_END;
	EFL_BLOCK_END;

	mutexTest.lock();
	ASSERT_TRUE(correct) << "Asynchronous operation with finally failed";

}
