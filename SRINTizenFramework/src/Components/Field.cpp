/*
 * Entry.cpp
 *
 *  Created on: Mar 3, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Framework/Application.h"
#include "SRIN/Components/Field.h"

#define FILE_EDC_FIELD "SRIN/edc/Field.edj"

LIBAPI Evas_Object* SRIN::Components::Field::CreateComponent(Evas_Object* root)
{
	Evas_Object* box;

	box = elm_layout_add(root);
	auto edj_path = Framework::ApplicationBase::GetResourcePath(FILE_EDC_FIELD);

	if (bottomBorder)
	{
		elm_layout_file_set(box, edj_path.c_str(), "main");
	}
	else
	{
		elm_layout_file_set(box, edj_path.c_str(), "no_border");
	}

	field = elm_entry_add(box);
	elm_entry_single_line_set(field, EINA_TRUE);
	elm_entry_scrollable_set(field, EINA_TRUE);
	elm_object_part_content_set(box, "content", field);

	if (text.length())
		elm_object_text_set(field, text.c_str());

	if (multiline)
	{
		elm_entry_single_line_set(field, EINA_FALSE);
		elm_entry_scrollable_set(field, EINA_FALSE);
		elm_entry_line_wrap_set(field, ELM_WRAP_MIXED);
	}
	else
	{
		elm_entry_single_line_set(field, EINA_TRUE);
		elm_entry_scrollable_set(field, EINA_TRUE);
	}

	elm_entry_editable_set(field, EINA_TRUE);
	evas_object_smart_callback_add(field, "activated", SmartEventHandler, &onReturnKeyClick);
	evas_object_smart_callback_add(field, "changed", SmartEventHandler, &onTextChanged);
	return box;
}

LIBAPI void SRIN::Components::Field::SetText(const std::string& text)
{
	this->text = text;

	if (field)
		elm_object_text_set(this->field, this->text.c_str());
}

LIBAPI std::string& SRIN::Components::Field::GetText()
{
	if (field)
	{
		auto textStr = elm_object_text_get(this->field);
		this->text = textStr;
	}

	return this->text;
}

LIBAPI void SRIN::Components::Field::SetMultiline(const bool& val)
{
	this->multiline = val;
	if (this->componentRoot)
	{
		if (multiline)
		{
			elm_entry_single_line_set(field, EINA_FALSE);
			elm_entry_scrollable_set(field, EINA_FALSE);
			elm_entry_line_wrap_set(field, ELM_WRAP_MIXED);
		}
		else
		{
			elm_entry_single_line_set(field, EINA_TRUE);
			elm_entry_scrollable_set(field, EINA_TRUE);
		}
	}
}

LIBAPI bool SRIN::Components::Field::GetMultiline()
{
	return multiline;
}

LIBAPI void SRIN::Components::Field::SetBottomBorderVisible(const bool& visible)
{
	bottomBorder = visible;
}

LIBAPI bool SRIN::Components::Field::GetBottomBorderVisible()
{
	return bottomBorder;
}

LIBAPI SRIN::Components::Field::Field() : BottomBorderVisible(this), Multiline(this), Text(this)
{
	field = nullptr;
	text = "";
	multiline = false;
	bottomBorder = false;
}

LIBAPI void SRIN::Components::Field::SetDisable(const bool& disable)
{
	if (disable)
	{
		elm_object_disabled_set(field, EINA_TRUE);
	}
	else
	{
		elm_object_disabled_set(field, EINA_FALSE);
	}
}

LIBAPI void SRIN::Components::Field::SetHint(const CString& hint)
{
	elm_object_part_text_set(field, "guide", hint);
}

LIBAPI void SRIN::Components::Field::SetFocus(const bool& disable)
{
	if (disable)
	{
		elm_object_focus_set(field, EINA_TRUE);
		elm_entry_input_panel_show_on_demand_set(field, EINA_FALSE);
	}
	else
	{
		elm_object_focus_set(field, EINA_FALSE);
		elm_entry_input_panel_show_on_demand_set(field, EINA_TRUE);
	}
}

LIBAPI void SRIN::Components::Field::SetFontStyle(const CString& style)
{
	elm_entry_text_style_user_push(field, style);
}

void SRIN::Components::Field::SetReturnKeyType(
		const Elm_Input_Panel_Return_Key_Type& type) {
	elm_entry_input_panel_return_key_type_set(field, type);

}

void SRIN::Components::Field::SetKeyboardType(
		const Elm_Input_Panel_Layout& type) {
	elm_entry_input_panel_layout_set(field, type);
}

LIBAPI void SRIN::Components::Field::SetAsPasswordField()
{
	elm_entry_password_set(field, EINA_TRUE);
}
