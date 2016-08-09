/*
 * Field.h
 *
 *  Created on: Mar 3, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_FIELD_H_
#define SRIN_FIELD_H_

#include "SRIN/Components/ComponentBase.h"

namespace SRIN {
namespace Components {
class LIBAPI Field: public ComponentBase
{
private:
	std::string text;
	bool multiline;
	bool bottomBorder;

	Evas_Object* field;
protected:
	virtual Evas_Object* CreateComponent(Evas_Object* root);
	void SetText(const std::string& text);
	std::string& GetText();
	void SetMultiline(const bool& val);
	bool GetMultiline();
	void SetBottomBorderVisible(const bool& visible);
	bool GetBottomBorderVisible();
public:
	Field();
	void SetDisable(const bool& disable);
	void SetHint(const CString& hint);
	void SetFocus(const bool& disable);
	void SetFontStyle(const CString& style);
	void SetReturnKeyType(const Elm_Input_Panel_Return_Key_Type& type);
	void SetKeyboardType(const Elm_Input_Panel_Layout& type);
	void SetAsPasswordField();

	EFL::EvasSmartEvent eventReturnKeyClick;
	EFL::EvasSmartEvent eventTextChanged;

	Property<Field, std::string&>::GetSet<&Field::GetText, &Field::SetText> Text;
	Property<Field, bool>::GetSet<&Field::GetMultiline, &Field::SetMultiline> Multiline;
	Property<Field, bool>::GetSet<&Field::GetBottomBorderVisible, &Field::SetBottomBorderVisible> BottomBorderVisible;
};
}
}

#endif /* FIELD_H_ */
