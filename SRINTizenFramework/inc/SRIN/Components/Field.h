/*
 * Field.h
 *
 *  Created on: Mar 3, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        ib.putu (ib.putu@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 *        Calvin Windoro (calvin.w@samsung.com)
 */

#ifndef SRIN_FIELD_H_
#define SRIN_FIELD_H_

#include "SRIN/Components/ComponentBase.h"

namespace SRIN {
namespace Components {
/**
 * Component that can be used as an entry field in forms.
 */
class LIBAPI Field: public ComponentBase
{
private:
	std::string text;
	bool multiline;
	bool bottomBorder;

	Evas_Object* field;
protected:
	/**
	 * Method overriden from ComponentBase, creates the UI elements of the component.
	 *
	 * @param root The root/parent given for this component.
	 *
	 * @return An Elm_Entry widget.
	 */
	virtual Evas_Object* CreateComponent(Evas_Object* root);
	void SetText(const std::string& text);
	std::string& GetText();
	void SetMultiline(const bool& val);
	bool GetMultiline();
	void SetBottomBorderVisible(const bool& visible);
	bool GetBottomBorderVisible();
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
	void SetDisable(const bool& disable);

	/**
	 * Set the field's hint text.
	 *
	 * @param hint The text that will be displayed as hint.
	 */
	void SetHint(const CString& hint);

	/**
	 * Set the field's focused status.
	 *
	 * @param focus If true, then field will receive focus and show its input panel.
	 * 				If false, then field will act as normal.
	 */
	void SetFocus(const bool& focus);

	/**
	 * Set the field's font style.
	 *
	 * @param style String that describes the style. Must conform to the EFL style format.
	 * 				In general, the format is specified as format tag='property=value'
	 * 				(i.e. DEFAULT='font=Sans font_size=60'hilight=' + font_weight=Bold').
	 */
	void SetFontStyle(const CString& style);

	/**
	 * Set the field's return key type in the input panel.
	 *
	 * @param type Enumeration that defines the various return key type.
	 * @see {Elm_Input_Panel_Return_Key_Type}
	 */
	void SetReturnKeyType(const Elm_Input_Panel_Return_Key_Type& type);

	/**
	 * Set the input panel type.
	 *
	 * @param type Enumeration that defines the input panel type.
	 * @see {Elm_Input_Panel_Layout}
	 */
	void SetKeyboardType(const Elm_Input_Panel_Layout& type);

	/**
	 * Set the field as password field (the characters will not be visible).
	 */
	void SetAsPasswordField();

	/**
	 * Event that will be triggered when user input return key.
	 */
	EFL::EvasSmartEvent eventReturnKeyClick;

	/**
	 * Event that will be triggered when text on the field changed.
	 */
	EFL::EvasSmartEvent eventTextChanged;

	/**
	 * Property that enables getting & setting text on the field.
	 * The return/parameter type is string reference.
	 */
	Property<Field, std::string&>::GetSet<&Field::GetText, &Field::SetText> Text;

	/**
	 * Property that enables getting & setting multiline mode of the field.
	 * The return/parameter type is bool.
	 */
	Property<Field, bool>::GetSet<&Field::GetMultiline, &Field::SetMultiline> Multiline;

	/**
	 * Property that enables getting & setting bottom border visibility of the field.
	 * The return/parameter type is bool.
	 */
	Property<Field, bool>::GetSet<&Field::GetBottomBorderVisible, &Field::SetBottomBorderVisible> BottomBorderVisible;
};
}
}

#endif /* FIELD_H_ */
