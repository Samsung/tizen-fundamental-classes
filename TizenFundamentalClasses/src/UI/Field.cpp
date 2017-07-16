/*
 * Field.cpp
 *
 *  Created on: Jul 15, 2017
 *      Author: gilang
 */

#include "TFC/UI/Field.h"
#include "TFC/Framework/Application.h"

#define FILE_EDC_FIELD "TFC/edc/Field.edj"

using namespace TFC::UI;

LIBAPI
Field::Field(Evas_Object* parent, bool bottomBorder) :
	WidgetBase(elm_layout_add(parent))
{
	auto edj_path = Framework::ApplicationBase::GetResourcePath(FILE_EDC_FIELD);

	if (bottomBorder)
		elm_layout_file_set(widgetRoot, edj_path.c_str(), "main");
	else
		elm_layout_file_set(widgetRoot, edj_path.c_str(), "no_border");

	field = elm_entry_add(widgetRoot);
	elm_object_part_content_set(widgetRoot, "content", field);

	elm_entry_single_line_set(field, EINA_TRUE);
	elm_entry_scrollable_set(field, EINA_TRUE);

	elm_entry_editable_set(field, EINA_TRUE);

	elm_entry_markup_filter_append(field, elm_entry_filter_limit_size, &limit_size);

	eventReturnKeyClick.Bind(field, "activated");
	eventTextChanged.Bind(field, "changed");
}

LIBAPI
void Field::SetHint(std::string const& hint)
{
	elm_object_part_text_set(field, "guide", hint.c_str());
}

LIBAPI
std::string Field::GetHint() const
{
	return elm_object_part_text_get(field, "guide");
}

LIBAPI
void Field::SetFocus(bool const& focus)
{
	if (focus)
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

LIBAPI
bool Field::GetFocus() const
{
	return elm_object_focus_get(this->field);
}

LIBAPI
void Field::SetFontStyle(const char* style)
{
	elm_entry_text_style_user_push(field, style);
}

LIBAPI
void Field::SetReturnKeyType(
		Elm_Input_Panel_Return_Key_Type const& type)
{
	elm_entry_input_panel_return_key_type_set(field, type);
}

LIBAPI
Elm_Input_Panel_Return_Key_Type Field::GetReturnKeyType() const
{
	return elm_entry_input_panel_return_key_type_get(field);
}

LIBAPI
void Field::SetKeyboardType(Elm_Input_Panel_Layout const& type)
{
	elm_entry_input_panel_layout_set(field, type);
}

LIBAPI
Elm_Input_Panel_Layout Field::GetKeyboardType() const
{
	return elm_entry_input_panel_layout_get(field);
}

LIBAPI
void Field::SetCursorAtEnd()
{
	elm_entry_cursor_end_set(field);
}

LIBAPI
void Field::SetCharLimit(int const& limit)
{
	limit_size.max_char_count = limit;

}

LIBAPI
int Field::GetCharLimit() const
{
	return limit_size.max_char_count;
}

LIBAPI
void Field::SetText(const std::string& text)
{
	elm_object_text_set(this->field, text.c_str());
}

LIBAPI
std::string Field::GetText() const
{
	return elm_object_text_get(this->field);
}

LIBAPI
void Field::SetMultiline(bool const& multiline)
{
	if (multiline)
	{
		elm_entry_single_line_set(this->field, EINA_FALSE);
		elm_entry_scrollable_set(this->field, EINA_FALSE);
		elm_entry_line_wrap_set(this->field, ELM_WRAP_MIXED);
	}
	else
	{
		elm_entry_single_line_set(this->field, EINA_TRUE);
		elm_entry_scrollable_set(this->field, EINA_TRUE);
	}
}

LIBAPI
bool Field::GetMultiline() const
{
	return elm_entry_single_line_get(this->field);
}

LIBAPI
void Field::SetPasswordMode(bool const& pwd)
{
	elm_entry_password_set(this->field, pwd);
}

LIBAPI
bool Field::GetPasswordMode() const
{
	return elm_entry_password_get(this->field);
}
