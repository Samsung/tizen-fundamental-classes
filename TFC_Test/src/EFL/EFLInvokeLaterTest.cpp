/*
 * EFLInvokeLaterTest.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: gilang
 */


#include "TFC/EFL.h"
#include <string>
#include <mutex>
#include <chrono>
#include <thread>
#include <gtest/gtest.h>

#include "TFC_Test.h"



class EFLInvokeLaterTest : public testing::Test
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

	class WhatComponent : public TFC::EFL::EFLProxyClass
	{
	public:
		int a;
		int b;
		int result;
		WhatComponent()
		{
			a = rand();
			b = rand();
			result = 0;
		}

		void JobToBeInvokedLater(int a, int b)
		{
			result = a * b;
		}

		void PerformInvokeLater()
		{
			InvokeLater(&WhatComponent::JobToBeInvokedLater, a, b);
		}
	};

}

TEST_F(EFLInvokeLaterTest, InvokeLaterWithParam)
{
	WhatComponent w;

	using Ms = std::chrono::milliseconds;

	EFL_BLOCK_BEGIN;
		EFL_SYNC_BEGIN(w);
			w.PerformInvokeLater();
		EFL_SYNC_END;
	EFL_BLOCK_END;

	std::this_thread::sleep_for(Ms(500));

	EXPECT_EQ(w.a * w.b, w.result);
}


