/*
 * IntrospectTest.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: Gilang M. Hamidy
 */

#include "TFC/Core.h"
#include "TFC/Core/Introspect.h"

#include <gtest/gtest.h>

class IntrospectTest : public testing::Test
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

void SomeFunction(int a, int b)
{

}

TEST_F(IntrospectTest, LambdaHasCallOperator)
{
	using namespace TFC::Core::Introspect;

	// If it is compiled then the declaration is correct
	std::string something("something");

	auto lambda = [something] () -> int { return 123; };
	ASSERT_TRUE(HasCallOperator<decltype(lambda)>::Value) << "HasCallOperator failed";

	auto nonCaptureLambda = [] (int a) -> bool { return false; };
	ASSERT_TRUE(HasCallOperator<decltype(nonCaptureLambda)>::Value) << "HasCallOperator failed on non-capture lambda";

	void (*someFuncPtr)(int, int) = SomeFunction;

	ASSERT_FALSE(HasCallOperator<void(int, int)>::Value) << "HasCallOperator failed on function pointer";

}

TEST_F(IntrospectTest, CallableType)
{
	using namespace TFC::Core::Introspect;

	std::string something("something");
	auto lambda = [something] () -> int { return 123; };
	ASSERT_TRUE(CallableObject<decltype(lambda)>::Callable) << "Lambda wasn't detected as callable";

	auto nonCapturingLambda = [] { 1 + 2; };
	ASSERT_TRUE(CallableObject<decltype(nonCapturingLambda)>::Callable) << "Non capturing lambda wasn't detected as callable";

}

