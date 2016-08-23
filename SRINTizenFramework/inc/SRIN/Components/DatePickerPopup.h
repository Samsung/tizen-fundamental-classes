/*
 * DatePickerPopup.h
 *
 *  Created on: Mar 16, 2016
 *      Author: Ida Bagus Putu Peradnya Dinata
 */

#ifndef DATEPICKERPOPUP_H_
#define DATEPICKERPOPUP_H_

#include "SRIN/Framework/Application.h"
#include "SRIN/Components/PopupBox.h"
#include "SRIN/Components/ComponentBase.h"
#include "SRIN/Components/BackButtonHandler.h"
#include <ctime>

namespace SRIN {
	namespace Components {
		/**
		 * Inner component of DatePickerPopup to create popup when date picker button was selected.
		 * Don't using this class in your tizen project, instead please use DatePickerPopup.
		 */
		class LIBAPI DatePickerPopupMenu : public PopupBox {
		private:
			bool 			classicTheme;
			Evas_Object* 	dateTime;
			std::tm      	selectedDate;
			std::string		strDate;
		protected:
			/**
			 * Method that can be overriden to create custom date picker PopupBox.
			 *
			 * @param root The root/parent given for this component.
			 *
			 * @return Evas_Object that contains the custom content.
			 */
			virtual Evas_Object* CreateContent(Evas_Object* root);
		public:
			/**
			 * Constructor of the DatePickerPopupMenu class.
			 */
			DatePickerPopupMenu();

			/**
			 * Method that return selected date.
			 *
			 * @return std::tm& tm struct of selected date/time.
			 */
			std::tm& 		GetSelectedDate();

			/**
			 * Method that return string of selected date.
			 *
			 * @return std::string& string of selected date/time.
			 */
			std::string& 	GetFormatedSelectedDate();

			/**
			 * Method that change UI of Date Picker.
			 *
			 * @param classic true if use classic (DatePicker) UI, or false to use modern (Calendar) UI.
			 */
			void UseClassicTheme(const bool& classic);

			/**
			 * Method that change init date value of datepicker
			 *
			 * @param date tm struct of selected date/time.
			 */
			void SetSelectedDate(const std::tm& date);

			/**
			 * Method that change init date value of datepicker.
			 * format: YYYY-MM-DD
			 *
			 * @param date The string of date.
			 */
			void SetFormatedSelectedDate(const std::string& date);

			/**
			 * Event that will triggered when date changed.
			 *
			 */
			EFL::EvasSmartEvent eventDateChanged;
		};

		/**
		 * Component that provides date picker popup.
		 * This component provide "classic" date picker popup using date picker UI, or
		 * modern date picker using calendar UI.
		 */
		class LIBAPI DatePickerPopup : public ComponentBase {
		private:
			Evas_Object* 	buttonRoot;

			DatePickerPopupMenu datePopupMenu;
			std::tm      		selectedDate;

			std::string 	 buttonText, formatedDate;

			EFL::EvasSmartEvent  eventButtonClick;

			void DatePickerButtonClick(EFL::EvasSmartEvent* viewSource, Evas_Object* objSource, void* eventData);
			void DatePickerPopupOkButtonClick(EFL::EvasSmartEvent* viewSource, Evas_Object* objSource, void* eventData);
			void DatePickerPopupCancelButtonClick(EFL::EvasSmartEvent* viewSource, Evas_Object* objSource, void* eventData);
			void OnDateChangedCb(EFL::EvasSmartEvent* viewSource, Evas_Object* objSource, void* eventData);
		protected:
			/**
			 * Method that can be overriden to create custom date picker PopupBox.
			 *
			 * @param root The root/parent given for this component.
			 *
			 * @return Evas_Object that contains the custom content.
			 */
			virtual Evas_Object* CreateComponent(Evas_Object* root) override;

			void SetTitle(const std::string& text);
			std::string& GetTitle();

			void SetHint(const std::string& text);
			std::string& GetHint();

			void SetOrientation(const Elm_Popup_Orient& orientation);
			Elm_Popup_Orient& GetOrientation();
		public:
			/**
			 * Constructor of the DatePickerPopup Component.
			 */
			DatePickerPopup();

			/**
			 * Property that enables getting & setting popup title.
			 * The return/parameter type is string.
			 */
			Property<DatePickerPopup, std::string&>::GetSet<&DatePickerPopup::GetTitle, &DatePickerPopup::SetTitle> Title;

			/**
			 * Property that enables getting & setting popup hint.
			 * The return/parameter type is string.
			 */
			Property<DatePickerPopup, std::string&>::GetSet<&DatePickerPopup::GetHint, &DatePickerPopup::SetHint> Hint;

			/**
			 * Property that enables getting & setting popup orientation.
			 * The return/parameter type is Elm_Popup_Orient.
			 * @see {Elm_Popup_Orient}
			 */
			Property<DatePickerPopup, Elm_Popup_Orient&>::GetSet<&DatePickerPopup::GetOrientation, &DatePickerPopup::SetOrientation> Orientation;

			/**
			 * Method that return selected date.
			 *
			 * @return std::tm& tm struct of selected date/time.
			 */
			std::tm& 		GetSelectedDate();

			/**
			 * Method that return string of selected date.
			 *
			 * @return std::string& string of selected date/time.
			 */
			std::string& 	GetFormatedSelectedDate();

			/**
			 * Method that change UI of Date Picker.
			 *
			 * @param classic true if use classic (DatePicker) UI, or false to use modern (Calendar) UI.
			 */
			void UseClassicTheme(const bool& classic);

			/**
			 * Method that change init date value of datepicker
			 *
			 * @param date tm struct of selected date/time.
			 */
			void SetSelectedDate(const std::tm& date);

			/**
			 * Method that change init date value of datepicker.
			 * format: YYYY-MM-DD
			 *
			 * @param date The string of date.
			 */
			void SetFormatedSelectedDate(const std::string& date);
		};
	}
}

#endif /* DATEPICKERPOPUP_H_ */
