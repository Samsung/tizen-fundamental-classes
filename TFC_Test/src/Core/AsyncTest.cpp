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
	auto operand = [a] (void*) { int b = a; } >> eventAsync;

	/* This is invalid operation because the lambda has parameter
	auto operandInvalid = [a] (int b) { int c = b + a; } >> eventAsync;
	*/


	//typedef TFC::Core::Async::AsyncOperand<void, decltype(eventAsync)> CorrectType;

	//ASSERT_TRUE((std::is_same<decltype(operand), CorrectType>::value)) << "Operator << yields incorrect type";
}

TEST_F(AsyncTest, OperatorRedirectionTestNonVoid)
{
	TFC::Core::SharedEventObject<TFC::Core::Async::AsyncTask<std::string>*, std::string> eventAsync;
	auto operand = [] (void*) { return std::string("Hello!"); } >> eventAsync;

	//typedef TFC::Core::Async::AsyncOperand<std::string, decltype(eventAsync)> CorrectType;
	//ASSERT_TRUE((std::is_same<decltype(operand), CorrectType>::value)) << "Operator << yields incorrect type on non void";
}

TEST_F(AsyncTest, LambdaToAsyncOperand)
{
	//TFC::Core::Async::AsyncOperand<void> operand([] { int a = 12 + 15; });

	//auto tmp = TFC::Core::Async::AsyncBuilder() & [] { return 5; };
}

class CopyInspector : public TFC::ManagedClass
{

};

class MoveBlocker
{
	int a;
public:
	MoveBlocker() : a (0) { }
	MoveBlocker(MoveBlocker&& other) = delete;
	MoveBlocker(MoveBlocker const& other) { this->a = other.a; }
	MoveBlocker(int val) : a(val) {}
	int GetA() const { return a; }
};

class AsyncTestClass : public TFC::EventClass
{
public:
	int testCase;
	int result;
	TFC::Async<int>::Event eventAsyncCompleted;
	std::timed_mutex mutexAsync;
	CopyInspector c;

	void OnAsyncCompleted(TFC::Async<int>::Task* task, int ret)
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
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Start Async Test");
		mutexAsync.lock();

		auto inspector = c.GetSafePointer();
		tfc_async {

			volatile bool isOk = inspector.TryAccess();
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

			tfc_async {

				int tmp = data.tc1;

				auto task = tfc_async {
					std::this_thread::sleep_for(Ms(2000));
					std::cout << "Finisih sleeping in thread\n";
					return tmp + 12345;
				};



				std::this_thread::sleep_for(Ms(4000));
				std::cout << "Finisih sleeping in outer\n";
				data.result = tfc_await task;

				std::cout << "Result here: " << data.result << std::endl;
			}
			tfc_async_complete
			{

			};
		EFL_SYNC_END;
	EFL_BLOCK_END;

	std::this_thread::sleep_for(Ms(6000));
	EXPECT_EQ(tc1 + 12345, result) << "Asynchronous completed but result is wrong";
}

TEST_F(AsyncTest, AsyncWithFinally)
{
	using namespace TFC;
	using Ms = std::chrono::milliseconds;
	std::mutex mutexTest;
	mutexTest.lock();
	bool correct = false;

	CopyInspector cp;

	dlog_print(DLOG_DEBUG, "TFC-Debug", "Start test Async with Finally");

	EFL_BLOCK_BEGIN;
		struct {
			bool& correct;
			int tc;
			std::mutex& mutexTest;
			CopyInspector& cp;
		} data = {
			correct,
			rand(),
			mutexTest,
			cp
		};

		EFL_SYNC_BEGIN(data);
			MoveBlocker mb(25);
			auto safeHandle = data.cp.GetSafePointer();
			tfc_async
			{
				return data.tc + mb.GetA();
			}
			tfc_async_complete(int result)
			{
				volatile bool val = safeHandle.TryAccess();
				data.correct = result == (data.tc + 25);
				data.mutexTest.unlock();
			};
		EFL_SYNC_END;
	EFL_BLOCK_END;

	mutexTest.lock();
	ASSERT_TRUE(correct) << "Asynchronous operation with finally failed";
}

TEST_F(AsyncTest, AsyncWithFinallyByReference)
{
	using namespace TFC;
	using Ms = std::chrono::milliseconds;
	std::mutex mutexTest;
	mutexTest.lock();
	bool correct = false;

	CopyInspector cp;

	dlog_print(DLOG_DEBUG, "TFC-Debug", "Start test Async with Finally");

	EFL_BLOCK_BEGIN;
		struct {
			bool& correct;
			int tc;
			std::mutex& mutexTest;
			CopyInspector& cp;
		} data = {
			correct,
			rand(),
			mutexTest,
			cp
		};

		EFL_SYNC_BEGIN(data);
			MoveBlocker mb(data.tc);
			tfc_async
			{
				return mb;
			}
			tfc_async_complete(MoveBlocker result)
			{
				data.correct = result.GetA() == data.tc;
				data.mutexTest.unlock();
			};
		EFL_SYNC_END;
	EFL_BLOCK_END;

	mutexTest.lock();
	ASSERT_TRUE(correct) << "Asynchronous operation with finally reference failed";
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

TEST_F(AsyncTest, AsyncWithSafeHandle)
{
	static bool operationResult = false;
	static std::mutex mutexAsync;
	mutexAsync.lock();

	using Ms = std::chrono::milliseconds;

	class SomeAsyncClass : public TFC::ManagedClass
	{
	public:
		bool safelyAccessed;

		SomeAsyncClass() : safelyAccessed(false) { }

		void DoAsync()
		{
			auto thisHandle = this->GetSafePointer();

			tfc_async
			{
				std::this_thread::sleep_for(Ms(2000));
				dlog_print(DLOG_DEBUG, "TFC-Debug", "Async task completed");
			}
			tfc_async_complete
			{
				if(!thisHandle)
					operationResult = true;
				else
					this->safelyAccessed = true;
				dlog_print(DLOG_DEBUG, "TFC-Debug", "Async complete completed");
				mutexAsync.unlock();
			};
		}
	};

	dlog_print(DLOG_DEBUG, "TFC-Debug", "Begin first case");

	int test = rand();
	EFL_BLOCK_BEGIN;
		EFL_SYNC_BEGIN(test);
			auto obj = new SomeAsyncClass;
			// Run the async
			obj->DoAsync();

			// Sleep a bit
			std::this_thread::sleep_for(Ms(100));

			// Delete the object
			delete obj;

		EFL_SYNC_END;
	EFL_BLOCK_END;

	// It will be notified after mutexAsync is unlocked by the complete handler
	mutexAsync.lock();
	ASSERT_TRUE(operationResult) << "Using SafePointer handle failed";

	dlog_print(DLOG_DEBUG, "TFC-Debug", "Begin second case");

	auto otherObj = new SomeAsyncClass;

	EFL_BLOCK_BEGIN;
		EFL_SYNC_BEGIN(otherObj);
			otherObj->DoAsync();
		EFL_SYNC_END;
	EFL_BLOCK_END;

	mutexAsync.lock();
	ASSERT_TRUE(otherObj->safelyAccessed) << "Using SafePointer handle failed";

	delete otherObj;
}

TEST_F(AsyncTest, AsyncWithSynchronizeBlock)
{
	using Ms = std::chrono::milliseconds;

	struct {
		std::mutex mutexTest;
		int test;
		int* result;
	} t;
	t.test = rand();
	t.result = new int;
	t.mutexTest.lock();

	EFL_BLOCK_BEGIN;
		EFL_SYNC_BEGIN(t);

			int test = t.test;
			int* result = t.result;
			std::mutex* mutexTest = &t.mutexTest;
			tfc_async
			{
				int value = test + 12345;
				dlog_print(DLOG_DEBUG, "TFC-Debug", "Before synchronize. Ctx: %d", __tfc_taskHandle );
				std::this_thread::sleep_for(Ms(1000));

				tfc_synchronize
				{
					value += 303030;
					std::this_thread::sleep_for(Ms(1000));
				};
				dlog_print(DLOG_DEBUG, "TFC-Debug", "After synchronize");
				return value;
			}
			tfc_async_complete(int value)
			{
				*result = value;
				mutexTest->unlock();
			};

		EFL_SYNC_END;
	EFL_BLOCK_END;

	t.mutexTest.lock();
	int expected = t.test + 12345 + 303030;

	EXPECT_EQ(expected, *t.result) << "Invalid result from synchronize";
	delete t.result;
}

static int counter = 0;

class DestructorDetector
{
public:

	DestructorDetector()
	{
		dlog_print(DLOG_DEBUG, "TFC-Destructor", "Constructed #%d %d", ++counter, this);
	}

	DestructorDetector(DestructorDetector&& o)
	{
		dlog_print(DLOG_DEBUG, "TFC-Destructor", "Move constructor #%d %d to %d", ++counter, &o, this);
	}

	DestructorDetector(DestructorDetector const& o)
	{
		dlog_print(DLOG_DEBUG, "TFC-Destructor", "Copy constructor #%d %d to %d", ++counter, &o, this);
	}

	~DestructorDetector()
	{
		dlog_print(DLOG_DEBUG, "TFC-Destructor", "Destructed #%d %d", counter--, this);
	}

	void SomeMethod()
	{
		volatile int var = 1;
		var += 5;
	}
};

TEST_F(AsyncTest, AsyncWithCatchBlock)
{
	using Ms = std::chrono::milliseconds;

	struct {
		std::mutex mutexTest;
		int test;
		int* result;
		bool* except;
	} t;
	t.test = rand();
	t.result = new int;
	t.mutexTest.lock();
	t.except = new bool;
	*t.except = false;
	std::cout << "Bool ptr: " << (int)t.except << '\n';

	EFL_BLOCK_BEGIN;
		EFL_SYNC_BEGIN(t);
			DestructorDetector d;
			int test = t.test;
			int* result = t.result;
			bool* except = t.except;
			std::mutex* mutexTest = &t.mutexTest;
			std::cout << "Bool ptr: " << (int)except << '\n';
			tfc_async
			{
				int value = test + 12345;
				dlog_print(DLOG_DEBUG, "TFC-Debug", "Before synchronize. Ctx: %d", __tfc_taskHandle );
				std::this_thread::sleep_for(Ms(1000));

				tfc_synchronize
				{
					value += 303030;
					std::this_thread::sleep_for(Ms(1000));
				};

				throw TFC::TFCException("There is an exception here");

				dlog_print(DLOG_DEBUG, "TFC-Debug", "After synchronize");
				return value;
			}
			tfc_async_complete(int value)
			{
				*result = value;
				mutexTest->unlock();
			}
			tfc_async_catch(TFC::TFCException const& err) mutable
			{
				d.SomeMethod();
				*(except) = true;
				dlog_print(DLOG_DEBUG, "TFC-Test", "Exception caught in TFC::Exception");
				std::cout << "Exception caught: " << err.what() << '\n';
				std::cout << "Bool ptr: " << (int)except << '\n';
				mutexTest->unlock();
			}
			tfc_async_catch(std::exception const& err)
			{
				*(except) = true;
				dlog_print(DLOG_DEBUG, "TFC-Test", "Exception caught in std::exception");
				mutexTest->unlock();
			};

		EFL_SYNC_END;
	EFL_BLOCK_END;
	std::this_thread::sleep_for(Ms(4000));
	t.mutexTest.try_lock();
	int expected = t.test + 12345 + 303030;

	ASSERT_TRUE(*(t.except)) << "Exception is not caught";
	delete t.result;
	delete t.except;
}

