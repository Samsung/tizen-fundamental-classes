/*
 * Field.h
 *
 *  Created on: Jul 15, 2017
 *      Author: gilang
 */

#ifndef TFC_UI_FIELD_H_
#define TFC_UI_FIELD_H_

#include "TFC/UI/WidgetBase.h"

namespace TFC {
namespace UI {

	class LIBAPI Field :
		public WidgetBase,
		EventEmitterClass<Field>,
		PropertyClass<Field>
	{
		using PropertyClass<Field>::Property;
		using EventEmitterClass<Field>::Event;

	public:
		Field(Evas_Object* parent, bool bottomBorder = true);



		void SetFocus(bool focus);

		/**
		 * Set the field's font style.
		 *
		 * @param style String that describes the style. Must conform to the EFL style format.
		 * 				In general, the format is specified as format tag='property=value'
		 * 				(i.e. DEFAULT='font=Sans font_size=60'hilight=' + font_weight=Bold').
		 */
		void SetFontStyle(char const* style);


		void SetCursorAtEnd();

		/**
		 * Event that will be triggered when user input return key.
		 */
		EvasSmartEvent eventReturnKeyClick;

		/**
		 * Event that will be triggered when text on the field changed.
		 */
		EvasSmartEvent eventTextChanged;

		TFC_Property(Field, std::string, Text);
		TFC_Property(Field, bool, Multiline);
		TFC_Property(Field, bool, PasswordMode);
		TFC_Property(Field, int, CharLimit);
		TFC_Property(Field, std::string, Hint);
		TFC_Property(Field, Elm_Input_Panel_Return_Key_Type, ReturnKeyType);
		TFC_Property(Field, Elm_Input_Panel_Layout, KeyboardType);

		/**
		 * Set the field's focused status.
		 */
		TFC_Property(Field, bool, Focus);

	private:
		Evas_Object* field;
		Elm_Entry_Filter_Limit_Size limit_size { INT_MAX, INT_MAX };
	};

}}



#endif /* TFC_UI_FIELD_H_ */
