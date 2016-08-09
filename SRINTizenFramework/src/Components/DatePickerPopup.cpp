/*
 * DatePickerPopup.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: Ida Bagus Putu Peradnya Dinata
 */

#include "SRIN/Components/DatePickerPopup.h"
#include <string>
#include <cstdio>

SRIN::Components::DatePickerPopupMenu::DatePickerPopupMenu() : PopupBox() {
	classicTheme 	= false;
	dateTime		= nullptr;
}

std::tm& SRIN::Components::DatePickerPopupMenu::GetSelectedDate() {
	if (classicTheme) {
		elm_datetime_value_get(dateTime, &selectedDate);
	} else {
		elm_calendar_selected_time_get(dateTime, &selectedDate);
	}

	return selectedDate;
}

std::string& SRIN::Components::DatePickerPopupMenu::GetFormatedSelectedDate() {
	char buffer[20];

	if (classicTheme) {
		elm_datetime_value_get(dateTime, &selectedDate);
	} else {
		elm_calendar_selected_time_get(dateTime, &selectedDate);
	}

	std::strftime(buffer, sizeof(buffer), "%F", &selectedDate);
	strDate = buffer;
	return strDate;
}

void SRIN::Components::DatePickerPopupMenu::UseClassicTheme(
		const bool& classic) {
	this->classicTheme = classic;
}

void SRIN::Components::DatePickerPopupMenu::SetSelectedDate(
		const std::tm& date) {

	selectedDate = date;
}

void SRIN::Components::DatePickerPopupMenu::SetFormatedSelectedDate(
		const std::string& date) {

	if (date.size() > 0) {
		int yearValue;
		int monthValue;
		int dateValue;
		std::sscanf(date.c_str(), "%4d-%2d-%2d", &yearValue, &monthValue, &dateValue);

		selectedDate.tm_year 	= yearValue - 1900;
		selectedDate.tm_mon 	= monthValue - 1;
		selectedDate.tm_mday 	= dateValue;
	}
}


Evas_Object* SRIN::Components::DatePickerPopupMenu::CreateContent(
		Evas_Object* root) {

	if (classicTheme) {
		dateTime = elm_datetime_add(root);

		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_DATE, EINA_TRUE);
		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_MONTH, EINA_TRUE);
		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_YEAR, EINA_TRUE);

		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_AMPM, EINA_FALSE);
		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_HOUR, EINA_FALSE);
		elm_datetime_field_visible_set(dateTime, ELM_DATETIME_MINUTE, EINA_FALSE);

		elm_object_style_set(dateTime, "date_layout");
		elm_datetime_format_set(dateTime, "%F");
		elm_datetime_value_set(dateTime, &selectedDate);

		evas_object_smart_callback_add(dateTime, "changed", &EFL::EvasSmartEventHandler, &eventDateChanged);
		evas_object_show(dateTime);
	} else {
		dateTime = elm_calendar_add(root);
		elm_calendar_selected_time_set(dateTime, &selectedDate);
		evas_object_smart_callback_add(dateTime, "changed", &EFL::EvasSmartEventHandler, &eventDateChanged);
		evas_object_show(dateTime);
	}

	return dateTime;
}

/** ============================================================================ **/

void SRIN::Components::DatePickerPopup::DatePickerButtonClick(
		EFL::EvasSmartEvent* viewSource, Evas_Object* objSource,
		void* eventData) {
	datePopupMenu.Show();
}

void SRIN::Components::DatePickerPopup::DatePickerPopupOkButtonClick(
		EFL::EvasSmartEvent* viewSource, Evas_Object* objSource,
		void* eventData) {
	formatedDate = datePopupMenu.GetFormatedSelectedDate();
	selectedDate = datePopupMenu.GetSelectedDate();
	elm_object_text_set(buttonRoot, formatedDate.c_str());
	datePopupMenu.Dismiss();
}

void SRIN::Components::DatePickerPopup::DatePickerPopupCancelButtonClick(
		EFL::EvasSmartEvent* viewSource, Evas_Object* objSource,
		void* eventData) {
	datePopupMenu.Dismiss();
}

void SRIN::Components::DatePickerPopup::OnDateChangedCb(
	EFL::EvasSmartEvent* viewSource, Evas_Object* objSource, void* eventData) {

}

Evas_Object* SRIN::Components::DatePickerPopup::CreateComponent(
		Evas_Object* root) {
	buttonRoot = elm_button_add(root);
	elm_object_text_set(buttonRoot, buttonText.c_str());
	evas_object_smart_callback_add(buttonRoot, "clicked", &EFL::EvasSmartEventHandler, &eventButtonClick);
	evas_object_show(this->buttonRoot);

	datePopupMenu.Create(root);

	return buttonRoot;
}

void SRIN::Components::DatePickerPopup::SetTitle(const std::string& text) {
	datePopupMenu.Title = text;
}

std::string& SRIN::Components::DatePickerPopup::GetTitle() {
	return datePopupMenu.Title;
}

SRIN::Components::DatePickerPopup::DatePickerPopup() : Title(this), Orientation(this), Hint(this) {
	buttonText			= "Select Date";

	std::time_t time	= std::time(nullptr);
	std::tm* time_tm	= std::localtime(&time);

	datePopupMenu.buttonOneText	= "OK";
	datePopupMenu.buttonTwoText = "Cancel";
	datePopupMenu.Orientation 	= Elm_Popup_Orient::ELM_POPUP_ORIENT_BOTTOM;
	datePopupMenu.UseClassicTheme(false);
	datePopupMenu.SetSelectedDate(*time_tm);

	buttonRoot					= nullptr;

	eventButtonClick 				 += AddEventHandler(DatePickerPopup::DatePickerButtonClick);
	datePopupMenu.eventButtonOneClick += AddEventHandler(DatePickerPopup::DatePickerPopupOkButtonClick);
	datePopupMenu.eventButtonTwoClick += AddEventHandler(DatePickerPopup::DatePickerPopupCancelButtonClick);
	datePopupMenu.eventDateChanged  += AddEventHandler(DatePickerPopup::OnDateChangedCb);

}

std::tm& SRIN::Components::DatePickerPopup::GetSelectedDate() {
	return selectedDate;
}

std::string& SRIN::Components::DatePickerPopup::GetFormatedSelectedDate() {
	return formatedDate;
}

void SRIN::Components::DatePickerPopup::SetSelectedDate(const std::tm& date) {
	datePopupMenu.SetSelectedDate(date);
}

void SRIN::Components::DatePickerPopup::SetFormatedSelectedDate(const std::string& date) {
	datePopupMenu.SetFormatedSelectedDate(date);
	//formatedDate = datePopupMenu.GetFormatedSelectedDate();
	//selectedDate = datePopupMenu.GetSelectedDate();
}

void SRIN::Components::DatePickerPopup::UseClassicTheme(const bool& classic) {
	datePopupMenu.UseClassicTheme(classic);
}

void SRIN::Components::DatePickerPopup::SetOrientation(const Elm_Popup_Orient& orientation) {
	datePopupMenu.Orientation = orientation;
}

Elm_Popup_Orient& SRIN::Components::DatePickerPopup::GetOrientation() {
	return datePopupMenu.Orientation;
}

void SRIN::Components::DatePickerPopup::SetHint(const std::string& text) {
	this->buttonText = text;
}

std::string& SRIN::Components::DatePickerPopup::GetHint() {
	return buttonText;
}
