/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Components/Field.cpp
 *
 * Created on:  Mar 3, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Ida Bagus Putu Peradnya Dinata (ib.putu@samsung.com)
 * 				Kevin Winata (k.winata@samsung.com)
 * 				Calvin Windoro (calvin.w@samsung.com)
 */

#include "TFC/Framework/Application.h"
#include "TFC/Components/Field.h"

#define FILE_EDC_FIELD "TFC/edc/Field.edj"

LIBAPI Evas_Object* TFC::Components::Field::CreateComponent(Evas_Object* root)
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

	/*
	evas_object_smart_callback_add(field, "activated", EFL::EvasSmartEventHandler, &eventReturnKeyClick);
	evas_object_smart_callback_add(field, "changed", EFL::EvasSmartEventHandler, &eventTextChanged);
	*/

	eventReturnKeyClick.Bind(field, "activated");
	eventTextChanged.Bind(field, "changed");

	return box;
}

LIBAPI void TFC::Components::Field::SetText(std::string const& text)
{
	this->text = text;

	if (field)
		elm_object_translatable_text_set(this->field, text.c_str());
		//elm_object_text_set(this->field, this->text.c_str());
}

LIBAPI std::string const& TFC::Components::Field::GetText() const
{
	if (field)
	{
		auto textStr = elm_object_text_get(this->field);
		this->text = textStr;
	}

	return this->text;
}

LIBAPI void TFC::Components::Field::SetMultiline(bool const& val)
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

LIBAPI bool TFC::Components::Field::GetMultiline() const
{
	return multiline;
}

LIBAPI void TFC::Components::Field::SetBottomBorderVisible(bool const& visible)
{
	bottomBorder = visible;
}

LIBAPI bool TFC::Components::Field::GetBottomBorderVisible() const
{
	return bottomBorder;
}

LIBAPI TFC::Components::Field::Field() :
		text(""), field(nullptr), multiline(false), bottomBorder(false)
{
}

/*
LIBAPI void TFC::Components::Field::SetDisable(bool const& disable)
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
*/

LIBAPI void TFC::Components::Field::SetHint(char const* hint)
{
	elm_object_part_text_set(field, "guide", hint);
}

LIBAPI void TFC::Components::Field::SetFocus(bool const& disable)
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

LIBAPI void TFC::Components::Field::SetFontStyle(char const* style)
{
	elm_entry_text_style_user_push(field, style);
}

void TFC::Components::Field::SetReturnKeyType(Elm_Input_Panel_Return_Key_Type const& type)
{
	elm_entry_input_panel_return_key_type_set(field, type);

}

void TFC::Components::Field::SetKeyboardType(Elm_Input_Panel_Layout const& type)
{
	elm_entry_input_panel_layout_set(field, type);
}

void TFC::Components::Field::SetCursorAtEnd()
{
	elm_entry_cursor_end_set(field);
}

LIBAPI void TFC::Components::Field::SetAsPasswordField()
{
	elm_entry_password_set(field, EINA_TRUE);
}

void TFC::Components::Field::SetGuideText(const std::string& text)
{
	this->guideText = text;

	if (field)
		elm_object_translatable_part_text_set(this->field, "guide", text.c_str());
}

const std::string& TFC::Components::Field::GetGuideText() const
{
	if (field)
	{
		auto textStr = elm_object_text_get(this->field);
		this->guideText = textStr;
	}

	return this->guideText;
}
