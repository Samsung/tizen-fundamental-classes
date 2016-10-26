/*
 * EventTest.cpp
 *
 *  Created on: Oct 25, 2016
 *      Author: Gilang M. Hamidy
 */

#include "TFC/Core.new.h"

#include <gtest/gtest.h>


class EventTest : public testing::Test
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

TEST_F(EventTest, EventDeclaration)
{
	ASSERT_TRUE(true);
}

TEST_F(EventTest, EventAssignment)
{
	ASSERT_TRUE(true);
}
