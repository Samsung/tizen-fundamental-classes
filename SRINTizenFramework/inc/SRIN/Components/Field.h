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
class Field: public ComponentBase
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
	bool& GetMultiline();
	void SetBottomBorderVisible(const bool& visible);
	bool& GetBottomBorderVisible();
public:
	Field();
	void SetDisable(const bool& disable);
	void SetHint(const CString& hint);
	void SetFocus(const bool& disable);
	void SetFontStyle(const CString& style);

	Property<Field, std::string, &Field::GetText, &Field::SetText> Text;
	Property<Field, bool, &Field::GetMultiline, &Field::SetMultiline> Multiline;
	Property<Field, bool, &Field::GetBottomBorderVisible, &Field::SetBottomBorderVisible> BottomBorderVisible;
};
}
}

#endif /* FIELD_H_ */
