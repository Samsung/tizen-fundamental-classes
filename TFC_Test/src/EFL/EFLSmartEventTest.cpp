/*
 * EFLSmartEventTest.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: Gilang M. Hamidy
 */

#include "TFC/EFL.new.h"
#include <string>
#include <mutex>
#include <chrono>
#include <thread>
#include <gtest/gtest.h>

#include "TFC_Test.h"

#define EFL_BLOCK_BEGIN \
{\
	std::mutex tfc__mtx;\
	tfc__mtx.lock();\

#define EFL_SYNC_BEGIN(SHARED_DATA) \
struct tfc__sharedStruct { std::mutex& mtx; decltype(SHARED_DATA)& shared; } \
tfc__sharedInst = { tfc__mtx, SHARED_DATA };	\
sync_thread([] (void* tfc__data) {\
	typedef decltype(SHARED_DATA) tfc__sharedType;\
	tfc__sharedStruct* tfc__sharedData = reinterpret_cast<tfc__sharedStruct*>(tfc__data);\
	tfc__sharedType& SHARED_DATA = tfc__sharedData->shared;

#define EFL_SYNC_END \
	tfc__sharedData->mtx.unlock();\
}, &tfc__sharedInst);\
std::lock_guard<std::mutex> tfc__guard(tfc__mtx);

#define EFL_BLOCK_END \
}

class EFLSmartEventTest : public testing::Test
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
	class SomeComponent : public TFC::EFL::EFLProxyClass
	{
	public:
		SomeComponent();
		void CreateComponent();
		void RaiseClick();
		void Unbind();
		void BindAgain();
		~SomeComponent();

		bool staticCallback, bindingCallback;
		bool exceptionHappened;
		Evas_Object* box;
		Evas_Object* button;
		Evas_Object* button2;
	private:

		EvasSmartEvent eventButtonClick;
		void OnButtonClick(EvasSmartEvent::Type* ev, Evas_Object* src, void* data);

	};

	SomeComponent::SomeComponent() :
		button				(nullptr),
		button2				(nullptr),
		staticCallback		(false),
		bindingCallback		(false),
		exceptionHappened	(false)
	{
		eventButtonClick += { this, &SomeComponent::OnButtonClick };
	}

	SomeComponent::~SomeComponent()
	{
		if(button)
		{
			EFL_BLOCK_BEGIN;

				Evas_Object* btn = button;

				EFL_SYNC_BEGIN(btn);
					evas_object_del(btn);
				EFL_SYNC_END;

			EFL_BLOCK_END;
		}
	}

	void SomeComponent::Unbind()
	{
		EFL_BLOCK_BEGIN;

			auto thiz = this;

			EFL_SYNC_BEGIN(thiz);
				thiz->eventButtonClick.Unbind();
			EFL_SYNC_END;

		EFL_BLOCK_END;
	}

	void SomeComponent::RaiseClick()
	{
		EFL_BLOCK_BEGIN;

			Evas_Object* btn = button;

			EFL_SYNC_BEGIN(btn);
				evas_object_smart_callback_call(btn, "clicked", nullptr);
			EFL_SYNC_END;

		EFL_BLOCK_END;
	}

	void SomeComponent::BindAgain()
	{
		EFL_BLOCK_BEGIN;

			auto thiz = this;

			EFL_SYNC_BEGIN(thiz);
				try
				{
					thiz->eventButtonClick.Bind(thiz->button2, "clicked");
				}
				catch(TFC::EFL::EventBoundException& ex)
				{
					thiz->exceptionHappened = true;
				}
			EFL_SYNC_END;
		EFL_BLOCK_END;
	}

	void SomeComponent::CreateComponent()
	{
		EFL_BLOCK_BEGIN;

			auto thiz = this;

			EFL_SYNC_BEGIN(thiz);
				auto box = elm_box_add(ad.conform);
				evas_object_size_hint_align_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				evas_object_size_hint_weight_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
				evas_object_show(box);
				attach_widget(box);
				thiz->box = box;

				auto button = elm_button_add(ad.conform);
				elm_object_text_set(button, "Tombol 1");
				evas_object_size_hint_align_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				evas_object_size_hint_weight_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
				evas_object_show(button);
				elm_box_pack_end(box, button);

				evas_object_smart_callback_add(button, "clicked", [] (void* data, Evas_Object* ev, void* ed) {
					auto thiz = reinterpret_cast<SomeComponent*>(data);
					thiz->staticCallback = true;
				}, thiz);
				thiz->button = button;
				thiz->eventButtonClick.Bind(button, "clicked");

				button = elm_button_add(ad.conform);
				elm_object_text_set(button, "Tombol 2");
				evas_object_size_hint_align_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				evas_object_size_hint_weight_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
				evas_object_show(button);
				elm_box_pack_end(box, button);
				thiz->button2 = button;
			EFL_SYNC_END;

		EFL_BLOCK_END;
	}

	void SomeComponent::OnButtonClick(EvasSmartEvent::Type* ev,
			Evas_Object* src, void* data) {
		this->bindingCallback = true;
	}
}

TEST_F(EFLSmartEventTest, EventDeclaration)
{
	ASSERT_TRUE(true);
}

/**
 * Test the mechanism of ataching event via TFC's event binding
 */
TEST_F(EFLSmartEventTest, SimpleWidget)
{
	SomeComponent s;

	s.CreateComponent();
	s.RaiseClick();

	EXPECT_EQ(true, s.staticCallback) << "Static callback failed";
	EXPECT_EQ(true, s.bindingCallback) << "Binding callback failed";
}

/**
 * Test the mechanism of exception when trying to bind an event which already been bound
 */
TEST_F(EFLSmartEventTest, BoundObjectException)
{
	SomeComponent s;
	s.CreateComponent();
	s.BindAgain();

	ASSERT_TRUE(s.exceptionHappened) << "Exception did not occur when binding a bound event";
}

/**
 * Test the mechanism of unbind event from object and rebind to another object
 */
TEST_F(EFLSmartEventTest, RebindEventToObject)
{
	SomeComponent s;
	s.CreateComponent();
	s.Unbind();
	s.BindAgain();


	ASSERT_FALSE(s.exceptionHappened) << "Exception occur when binding an unbound event";
}
