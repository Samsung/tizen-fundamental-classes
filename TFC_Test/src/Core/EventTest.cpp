/*
 * EventTest.cpp
 *
 *  Created on: Oct 25, 2016
 *      Author: Gilang M. Hamidy
 */

#include "TFC/Core.new.h"
#include <string>
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

namespace {
	class EventEmitterA : public TFC::EventEmitterClass<EventEmitterA>
	{
	public:
		Event<std::string> eventSomethingA;
		void RaiseSomethingA(int what)
		{
			std::string tmp = std::to_string(what);
			eventSomethingA(this, tmp);
		}
	};

	class EventHandlerClass : public TFC::EventClass
	{
	public:
		EventEmitterA obj;
		std::string result;

		EventHandlerClass()
		{
			obj.eventSomethingA += { this, &EventHandlerClass::OnSomethingA };
		}

		void OnSomethingA(EventEmitterA::Event<std::string>* ev, EventEmitterA* source, std::string data)
		{
			result = data;
		}

		void PerformSomething(int what)
		{
			obj.RaiseSomethingA(what);
		}

		void DetachEvent()
		{
			obj.eventSomethingA -= { this, &EventHandlerClass::OnSomethingA };
		}
	};

	class EventEmitterB : public TFC::EventEmitterClass<EventEmitterB>
	{
	public:
		Event<std::string>* ptr;
		TFC::Core::SharedEventObject<std::string, std::string> eventShared;

		EventEmitterB()
		{
			//ptr = new Event<std::string>();
		}
	};
}

TEST_F(EventTest, EventDeclaration)
{
	ASSERT_TRUE(true);
}

TEST_F(EventTest, EventAssignment)
{
	EventHandlerClass eh;

	int data = rand();
	std::string expected = std::to_string(data);

	EXPECT_NE(expected, eh.result) << "Result string somehow equal";
	eh.PerformSomething(data);
	EXPECT_EQ(expected, eh.result) << "Result string not equal";

	// Try to detach event
	eh.DetachEvent();
	data = rand();
	expected = std::to_string(data);

	eh.PerformSomething(data);
	EXPECT_NE(expected, eh.result) << "Result string still same after detach event";

}

TEST_F(EventTest, AllocatingEventOnHeap)
{

}
