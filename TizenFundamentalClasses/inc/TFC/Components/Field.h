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
 *    Components/Field.h
 *
 * Components which wraps EFL Entry widget
 *
 * Created on:  Mar 3, 2016
 * Author:      Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Ida Bagus Putu Peradnya Dinata (ib.putu@samsung.com)
 *              Kevin Winata (k.winata@samsung.com)
 *              Calvin Windoro (calvin.w@samsung.com)
 */

#ifndef TFC_FIELD_H_
#define TFC_FIELD_H_

#include "TFC/Components/ComponentBase.h"

namespace TFC {
namespace Components {
/**
 * Component that can be used as an entry field in forms.
 */
class LIBAPI Field :
		public ComponentBase,
		EventEmitterClass<Field>,
		PropertyClass<Field>
{
	using PropertyClass<Field>::Property;
	using EventEmitterClass<Field>::Event;

public:
	/**
	 * Constructor for Field component.
	 */
	Field();

	/**
	 * Set the field's disabled status.
	 *
	 * @param disable If true, then the field will be disabled (can't be inputted to).
	 * 				  If false, then the field will be restored to normal.
	 */
	//void SetDisable(bool const& disable);

	/**
	 * Set the field's hint text.
	 *
	 * @param hint The text that will be displayed as hint.
	 */
	void SetHint(char const* hint);

	/**
	 * Set the field's focused status.
	 *
	 * @param focus If true, then field will receive focus and show its input panel.
	 * 				If false, then field will act as normal.
	 */
	void SetFocus(bool const& focus);

	/**
	 * Set the field's font style.
	 *
	 * @param style String that describes the style. Must conform to the EFL style format.
	 * 				In general, the format is specified as format tag='property=value'
	 * 				(i.e. DEFAULT='font=Sans font_size=60'hilight=' + font_weight=Bold').
	 */
	void SetFontStyle(char const* style);

	/**
	 * Set the field's return key type in the input panel.
	 *
	 * @param type Enumeration that defines the various return key type.
	 * @see {Elm_Input_Panel_Return_Key_Type}
	 */
	void SetReturnKeyType(Elm_Input_Panel_Return_Key_Type const& type);

	/**
	 * Set the input panel type.
	 *
	 * @param type Enumeration that defines the input panel type.
	 * @see {Elm_Input_Panel_Layout}
	 */
	void SetKeyboardType(Elm_Input_Panel_Layout const& type);

	/**
	 * Set the field as password field (the characters will not be visible).
	 */
	void SetAsPasswordField();

	void SetCursorAtEnd();

	/**
	 * Event that will be triggered when user input return key.
	 */
	EvasSmartEvent eventReturnKeyClick;

	/**
	 * Event that will be triggered when text on the field changed.
	 */
	EvasSmartEvent eventTextChanged;
private:
	mutable std::string text;
	mutable std::string guideText;

	Evas_Object* field;

	bool multiline;
	bool bottomBorder;
protected:
	/**
	 * Method overriden from ComponentBase, creates the UI elements of the component.
	 *
	 * @param root The root/parent given for this component.
	 *
	 * @return An Elm_Entry widget.
	 */
	virtual Evas_Object* CreateComponent(Evas_Object* root) override;
	void SetText(std::string const& text);
	std::string const& GetText() const;


	void SetGuideText(std::string const& text);
	std::string const& GetGuideText() const;

	void SetMultiline(bool const& val);
	bool GetMultiline() const;
	void SetBottomBorderVisible(bool const& visible);
	bool GetBottomBorderVisible() const;
public:
	/**
	 * Property that enables getting & setting text on the field.
	 * The return/parameter type is string reference.
	 */
	Property<std::string const&>::Get<&Field::GetText>::Set<&Field::SetText> Text { this };

	/**
	 * Property that enables getting & setting multiline mode of the field.
	 * The return/parameter type is bool.
	 */
	Property<bool>::Get<&Field::GetMultiline>::Set<&Field::SetMultiline> Multiline { this };

	/**
	 * Property that enables getting & setting bottom border visibility of the field.
	 * The return/parameter type is bool.
	 */
	Property<bool>::Get<&Field::GetBottomBorderVisible>::Set<&Field::SetBottomBorderVisible> BottomBorderVisible { this };

	/**
	 * Property that enables getting & setting bottom border visibility of the field.
	 * The return/parameter type is bool.
	 */
	Property<std::string const&>::Get<&Field::GetGuideText>::Set<&Field::SetGuideText> GuideText { this };
};
}
}

#endif /* FIELD_H_ */
