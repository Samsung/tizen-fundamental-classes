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

		class LIBAPI DatePickerPopupMenu : public PopupBox {
		private:
			bool 			classicTheme;
			Evas_Object* 	dateTime;
			std::tm      	selectedDate;
			std::string		strDate;
		protected:
			virtual Evas_Object* CreateContent(Evas_Object* root);
		public:
			DatePickerPopupMenu();

			std::tm& 		GetSelectedDate();
			std::string& 	GetFormatedSelectedDate();

			void UseClassicTheme(const bool& classic);
			void SetSelectedDate(const std::tm& date);
			void SetFormatedSelectedDate(const std::string& date);

			EFL::EvasSmartEvent eventDateChanged;
		};

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
			virtual Evas_Object* CreateComponent(Evas_Object* root) override;

			void SetTitle(const std::string& text);
			std::string& GetTitle();

			void SetHint(const std::string& text);
			std::string& GetHint();

			void SetOrientation(const Elm_Popup_Orient& orientation);
			Elm_Popup_Orient& GetOrientation();
		public:
			DatePickerPopup();

			Property<DatePickerPopup, std::string&>::GetSet<&DatePickerPopup::GetTitle, &DatePickerPopup::SetTitle> Title;
			Property<DatePickerPopup, std::string&>::GetSet<&DatePickerPopup::GetHint, &DatePickerPopup::SetHint> Hint;
			Property<DatePickerPopup, Elm_Popup_Orient&>::GetSet<&DatePickerPopup::GetOrientation, &DatePickerPopup::SetOrientation> Orientation;

			std::tm& 		GetSelectedDate();
			std::string& 	GetFormatedSelectedDate();

			void UseClassicTheme(const bool& classic);
			void SetSelectedDate(const std::tm& date);
			void SetFormatedSelectedDate(const std::string& date);
		};
	}
}

#endif /* DATEPICKERPOPUP_H_ */
