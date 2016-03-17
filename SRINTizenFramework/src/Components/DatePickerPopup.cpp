/*
 * DatePickerPopup.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: Ida Bagus Putu Peradnya Dinata
 */

#include "SRIN/Components/DatePickerPopup.h"
#include <string>
#include <cstdio>

LIBAPI void SRIN::Components::DatePickerPopup::DatePickerButtonClick(
		ElementaryEvent* viewSource, Evas_Object* objSource,
		void* eventData) {
	DatePickerPopup::ShowDatePickerPopup();
}

LIBAPI void SRIN::Components::DatePickerPopup::DatePickerPopupOkButtonClick(
		ElementaryEvent* viewSource, Evas_Object* objSource,
		void* eventData) {

	if(popupWindow) {
		char 		buffer[20];

		if (dateTime) {
			if (classicTheme) {
				elm_datetime_value_get(dateTime, &selectedDate);
			} else {
				elm_calendar_selected_time_get(dateTime, &selectedDate);
			}
		}

		std::strftime(buffer, sizeof(buffer), "%F", &selectedDate);
		formatedDate = buffer;

		buttonText = formatedDate;
		elm_object_text_set(buttonRoot, buttonText.c_str());

		evas_object_del(popupWindow);
		popupWindow = nullptr;
	}
}

LIBAPI void SRIN::Components::DatePickerPopup::DatePickerPopupCancelButtonClick(
		ElementaryEvent* viewSource, Evas_Object* objSource,
		void* eventData) {
	if(popupWindow) {
		evas_object_del(popupWindow);
		popupWindow = nullptr;
	}
}

LIBAPI void SRIN::Components::DatePickerPopup::OnDateChangedCb(
		ElementaryEvent* viewSource, Evas_Object* objSource, void* eventData) {

}

LIBAPI void SRIN::Components::DatePickerPopup::ShowDatePickerPopup() {
	this->popupWindow = elm_popup_add(this->layoutRoot);
	elm_object_style_set(this->popupWindow, "popup");

	std::string popup_title = "<align=center><b>";
	popup_title.append(title);
	popup_title.append("</b></align>");

	elm_object_part_text_set(this->popupWindow, "title,text", popup_title.c_str());
	elm_popup_allow_events_set(this->popupWindow, EINA_FALSE); //set as modal

	if (classicTheme) {
		dateTime = elm_datetime_add(this->popupWindow);

		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_DATE, EINA_TRUE);
		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_MONTH, EINA_TRUE);
		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_YEAR, EINA_TRUE);

		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_AMPM, EINA_FALSE);
		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_HOUR, EINA_FALSE);
		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_MINUTE, EINA_FALSE);

		elm_object_style_set(dateTime, "date_layout");
		elm_datetime_format_set(dateTime, "%F");
		elm_datetime_value_set(dateTime, &selectedDate);

		evas_object_smart_callback_add(dateTime, "changed", &SmartEventHandler, &onDateChanged);

		elm_object_content_set(this->popupWindow, dateTime);
		evas_object_show(dateTime);
	} else {
		dateTime = elm_calendar_add(this->popupWindow);
		elm_calendar_selected_time_set(dateTime, &selectedDate);

		evas_object_smart_callback_add(dateTime, "changed", &SmartEventHandler, &onDateChanged);

		elm_object_content_set(this->popupWindow, dateTime);
		evas_object_show(dateTime);
	}

	Evas_Object* buttonCancel;
	Evas_Object* buttonOK;

	buttonCancel = elm_button_add(this->popupWindow);
	elm_object_style_set(buttonCancel, "popup");
	elm_object_text_set(buttonCancel, buttonCancelText.c_str());

	buttonOK = elm_button_add(this->popupWindow);
	elm_object_style_set(buttonOK, "popup");
	elm_object_text_set(buttonOK, buttonOkText.c_str());

	elm_object_part_content_set(this->popupWindow, "button1", buttonCancel);
	elm_object_part_content_set(this->popupWindow, "button2", buttonOK);

	evas_object_smart_callback_add(buttonCancel, "clicked", &SmartEventHandler, &popupCancelClick);
	//eext_object_event_callback_add(this->popupWindow, EEXT_CALLBACK_BACK, &SmartEventHandler, &popupCancelClick);
	evas_object_smart_callback_add(buttonOK, "clicked", &SmartEventHandler, &popupOkClick);

	elm_popup_align_set(this->popupWindow, ELM_NOTIFY_ALIGN_FILL, 1.0);
	evas_object_show(this->popupWindow);
}

LIBAPI Evas_Object* SRIN::Components::DatePickerPopup::CreateComponent(
		Evas_Object* root) {
	layoutRoot = root;
	buttonRoot = elm_button_add(root);
	elm_object_text_set(buttonRoot, buttonText.c_str());
	evas_object_smart_callback_add(buttonRoot, "clicked", &SmartEventHandler, &buttonClick);
	evas_object_show(this->buttonRoot);

	return buttonRoot;
}

LIBAPI bool SRIN::Components::DatePickerPopup::BackButtonPressed() {
}

LIBAPI void SRIN::Components::DatePickerPopup::SetTitle(const std::string& text) {
	title = text;
}

LIBAPI std::string& SRIN::Components::DatePickerPopup::GetTitle() {
	return title;
}

LIBAPI SRIN::Components::DatePickerPopup::DatePickerPopup() : Title(this) {
	buttonText			= "Select";
	title				= "Date Picker";
	formatedDate		= "";
	buttonOkText		= "OK";
	buttonCancelText	= "Cancel";

	classicTheme		= false;

	std::time_t time	= std::time(nullptr);
	std::tm* time_tm	= std::localtime(&time);
	selectedDate		= *time_tm;

	buttonRoot			= nullptr;
	popupWindow			= nullptr;
	dateTime			= nullptr;
	layoutRoot			= nullptr;

	buttonClick 		+= { this, &DatePickerPopup::DatePickerButtonClick };
	onDateChanged 		+= { this, &DatePickerPopup::OnDateChangedCb };
	popupOkClick		+= { this, &DatePickerPopup::DatePickerPopupOkButtonClick };
	popupCancelClick	+= { this, &DatePickerPopup::DatePickerPopupCancelButtonClick };
}

LIBAPI std::tm& SRIN::Components::DatePickerPopup::GetSelectedDateTM() {
	return selectedDate;
}

LIBAPI std::string& SRIN::Components::DatePickerPopup::GetFormatedSelectedDate() {
	return formatedDate;
}

LIBAPI void SRIN::Components::DatePickerPopup::SetSelectedDateTM(const std::tm& date) {
	this->selectedDate = date;

	if (classicTheme) {
		elm_datetime_value_set(dateTime, &selectedDate);
	} else {
		elm_calendar_selected_time_set(dateTime, &selectedDate);
	}
}

LIBAPI void SRIN::Components::DatePickerPopup::SetFormatedSelectedDate(const std::string& date) {
	if (date.size() > 0) {
		int yearValue;
		int monthValue;
		int dateValue;
		std::sscanf(date.c_str(), "%4d-%2d-%2d", &yearValue, &monthValue, &dateValue);

		selectedDate.tm_year 	= yearValue - 1900;
		selectedDate.tm_mon 	= monthValue-1;
		selectedDate.tm_mday 	= dateValue;

		if (classicTheme) {
			elm_datetime_value_set(dateTime, &selectedDate);
		} else {
			elm_calendar_selected_time_set(dateTime, &selectedDate);
		}
	}
}

LIBAPI void SRIN::Components::DatePickerPopup::UseClassicTheme(const bool& classic) {
	classicTheme = classic;
}
