/*
 * DatePickerPopup.h
 *
 *  Created on: Mar 16, 2016
 *      Author: Ida Bagus Putu Peradnya Dinata
 */

#ifndef DATEPICKERPOPUP_H_
#define DATEPICKERPOPUP_H_

#include "SRIN/Framework/Application.h"
#include "SRIN/Components/ComponentBase.h"
#include <ctime>

namespace SRIN {
	namespace Components {
		class LIBAPI DatePickerPopup : public ComponentBase {
		private:
			bool classicTheme;
			std::tm      selectedDate;
			Evas_Object* buttonRoot;
			Evas_Object* popupWindow;
			Evas_Object* dateTime;
			Evas_Object* layoutRoot;

			std::string 		buttonText, title, formatedDate, buttonOkText, buttonCancelText;
			ElementaryEvent 	buttonClick, popupOkClick, onDateChanged, popupCancelClick;

			void DatePickerButtonClick(ElementaryEvent* viewSource, Evas_Object* objSource, void* eventData);
			void DatePickerPopupOkButtonClick(ElementaryEvent* viewSource, Evas_Object* objSource, void* eventData);
			void DatePickerPopupCancelButtonClick(ElementaryEvent* viewSource, Evas_Object* objSource, void* eventData);
			void OnDateChangedCb(ElementaryEvent* viewSource, Evas_Object* objSource, void* eventData);

			void ShowDatePickerPopup();
		protected:
			virtual Evas_Object* CreateComponent(Evas_Object* root) override;
			virtual bool BackButtonPressed();

			void SetTitle(const std::string& text);
			std::string& GetTitle();
		public:
			DatePickerPopup();

			Property<DatePickerPopup, std::string&>::GetSet<&DatePickerPopup::GetTitle, &DatePickerPopup::SetTitle> Title;

			std::tm& 		GetSelectedDateTM();
			std::string& 	GetFormatedSelectedDate();

			void UseClassicTheme(const bool& classic);
			void SetSelectedDateTM(const std::tm& date);
			void SetFormatedSelectedDate(const std::string& date);
		};
	}
}

#endif /* DATEPICKERPOPUP_H_ */
