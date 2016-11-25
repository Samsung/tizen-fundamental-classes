/*
 * ValidatorTest.cpp
 *
 *  Created on: Nov 24, 2016
 *      Author: Kevin Winata
 */

#include "TFC/Components/Validators/FieldValidator.h"
#include <gtest/gtest.h>
#include <mutex>

#include "TFC_Test.h"


class ValidatorTest : public testing::Test
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
class SomeView
{
public:
	SomeView();
	void CreateView();
	~SomeView();

	std::string text;
	void SetText();

	void AddEmptyValidation();
	void AddLengthValidation();
	void AddEmailValidation();

	TFC::Components::Validators::ValidatorList validators;
	TFC::Components::Field field;
};

SomeView::SomeView()
{
}

void SomeView::CreateView()
{
	EFL_BLOCK_BEGIN;
		auto& fieldRef = field;
		EFL_SYNC_BEGIN(fieldRef);
			auto box = elm_box_add(ad.conform);
			evas_object_size_hint_align_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_weight_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
			evas_object_show(box);
			attach_widget(box);

			fieldRef.Create(box);
			fieldRef.Name = "TestField";
			fieldRef.Text = "";
			elm_box_pack_end(box, fieldRef);
		EFL_SYNC_END;
	EFL_BLOCK_END;
}

SomeView::~SomeView()
{
}

void SomeView::SetText()
{
	EFL_BLOCK_BEGIN
		auto thiz = this;
		EFL_SYNC_BEGIN(thiz)
			thiz->field.Text = thiz->text;
		EFL_SYNC_END;
	EFL_BLOCK_END;
}

void SomeView::AddEmptyValidation()
{
	EFL_BLOCK_BEGIN
		auto thiz = this;
		EFL_SYNC_BEGIN(thiz)
			auto emptyValidator = new TFC::Components::Validators::EmptyFieldValidator(&(thiz->field));
			thiz->validators.AddValidator(emptyValidator);
		EFL_SYNC_END;
	EFL_BLOCK_END;
}

void SomeView::AddLengthValidation()
{
	EFL_BLOCK_BEGIN
		auto thiz = this;
		EFL_SYNC_BEGIN(thiz)
			auto lengthValidator = new TFC::Components::Validators::LengthValidator(&(thiz->field), 3, 5);
			thiz->validators.AddValidator(lengthValidator);
		EFL_SYNC_END;
	EFL_BLOCK_END;
}

void SomeView::AddEmailValidation()
{
	EFL_BLOCK_BEGIN
		auto thiz = this;
		EFL_SYNC_BEGIN(thiz)
			auto emailValidator = new TFC::Components::Validators::EmailValidator(&(thiz->field));
			thiz->validators.AddValidator(emailValidator);
		EFL_SYNC_END;
	EFL_BLOCK_END;
}
}

TEST_F(ValidatorTest, ZeroValidation)
{
	SomeView view;
	view.CreateView();
	ASSERT_TRUE(view.validators.ValidateAll());
}

TEST_F(ValidatorTest, EmptyFieldValidation)
{
	std::cout << "\n";
	SomeView view;
	view.CreateView();
	view.AddEmptyValidation();

	view.text = ""; view.SetText();
	ASSERT_FALSE(view.validators.ValidateAll());
	std::cout << view.validators.LastValidationMessage << "\n";

	view.text = "abc"; view.SetText();
	ASSERT_TRUE(view.validators.ValidateAll());
}

TEST_F(ValidatorTest, LengthFieldValidation)
{
	std::cout << "\n";
	SomeView view;
	view.CreateView();
	view.AddLengthValidation();

	view.text = "abcdef"; view.SetText();
	ASSERT_FALSE(view.validators.ValidateAll());
	std::cout << view.validators.LastValidationMessage << "\n";

	view.text = "ab"; view.SetText();
	ASSERT_FALSE(view.validators.ValidateAll());
	std::cout << view.validators.LastValidationMessage << "\n";

	view.text = "abcd"; view.SetText();
	ASSERT_TRUE(view.validators.ValidateAll());
}

TEST_F(ValidatorTest, EmailFieldValidation)
{
	std::cout << "\n";
	SomeView view;
	view.CreateView();
	view.AddEmailValidation();

	view.text = "abcmail.com"; view.SetText();
	ASSERT_FALSE(view.validators.ValidateAll());
	std::cout << view.validators.LastValidationMessage << "\n";

	view.text = "abc@mail.com"; view.SetText();
	ASSERT_TRUE(view.validators.ValidateAll());
}
