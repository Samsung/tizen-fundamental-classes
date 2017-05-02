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
 *    Components/DatePickerPopup.cpp
 *
 * Created on:  Mar 16, 2016
 * Author: 		Ida Bagus Putu Peradnya Dinata (ib.putu@samsung.com)
 * Contributor: Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * 				Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/DatePickerPopup.h"
#include <string>
#include <cstdio>

TFC::Components::DatePickerPopupMenu::DatePickerPopupMenu() : PopupBox() {
	classicTheme 	= false;
	dateTime		= nullptr;
}

std::tm const& TFC::Components::DatePickerPopupMenu::GetSelectedDate() {
	if (classicTheme) {
		elm_datetime_value_get(dateTime, &selectedDate);
	} else {
		elm_calendar_selected_time_get(dateTime, &selectedDate);
	}

	return selectedDate;
}

std::string const& TFC::Components::DatePickerPopupMenu::GetFormatedSelectedDate() {
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

void TFC::Components::DatePickerPopupMenu::UseClassicTheme(
		bool const& classic) {
	this->classicTheme = classic;
}

void TFC::Components::DatePickerPopupMenu::SetSelectedDate(
		std::tm const& date) {

	selectedDate = date;
}

void TFC::Components::DatePickerPopupMenu::SetFormatedSelectedDate(
		std::string const& date) {

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


Evas_Object* TFC::Components::DatePickerPopupMenu::CreateContent(
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

		//evas_object_smart_callback_add(dateTime, "changed", &EFL::EvasSmartEventHandler, &eventDateChanged);
		eventDateChanged.Bind(dateTime, "changed");
		evas_object_show(dateTime);
	} else {
		dateTime = elm_calendar_add(root);
		elm_calendar_selected_time_set(dateTime, &selectedDate);
		//evas_object_smart_callback_add(dateTime, "changed", &EFL::EvasSmartEventHandler, &eventDateChanged);
		eventDateChanged.Bind(dateTime, "changed");
		evas_object_show(dateTime);
	}

	return dateTime;
}

/** ============================================================================ **/

void TFC::Components::DatePickerPopup::DatePickerButtonClick(Evas_Object* objSource, void* eventData)
{
	datePopupMenu.Show();
}

void TFC::Components::DatePickerPopup::DatePickerPopupOkButtonClick(Evas_Object* objSource, void* eventData)
{
	formatedDate = datePopupMenu.GetFormatedSelectedDate();
	selectedDate = datePopupMenu.GetSelectedDate();
	elm_object_text_set(buttonRoot, formatedDate.c_str());
	datePopupMenu.Dismiss();
}

void TFC::Components::DatePickerPopup::DatePickerPopupCancelButtonClick(Evas_Object* objSource, void* eventData)
{
	datePopupMenu.Dismiss();
}

void TFC::Components::DatePickerPopup::OnDateChangedCb(Evas_Object* objSource, void* eventData)
{

}

Evas_Object* TFC::Components::DatePickerPopup::CreateComponent(
		Evas_Object* root) {
	buttonRoot = elm_button_add(root);
	elm_object_text_set(buttonRoot, buttonText.c_str());
	//evas_object_smart_callback_add(buttonRoot, "clicked", &EFL::EvasSmartEventHandler, &eventButtonClick);
	eventButtonClick.Bind(buttonRoot, "clicked");
	evas_object_show(this->buttonRoot);

	datePopupMenu.Create(root);

	return buttonRoot;
}

void TFC::Components::DatePickerPopup::SetTitle(std::string const& text) {
	datePopupMenu.Title = text;
}

std::string const& TFC::Components::DatePickerPopup::GetTitle() const {
	return datePopupMenu.Title;
}

TFC::Components::DatePickerPopup::DatePickerPopup() : Title(this), Hint(this), Orientation(this) {
	buttonText			= "Select Date";

	std::time_t time	= std::time(nullptr);
	std::tm* time_tm	= std::localtime(&time);

	datePopupMenu.ButtonOneText	= "OK";
	datePopupMenu.ButtonTwoText = "Cancel";
	datePopupMenu.Orientation 	= Elm_Popup_Orient::ELM_POPUP_ORIENT_BOTTOM;
	datePopupMenu.UseClassicTheme(false);
	datePopupMenu.SetSelectedDate(*time_tm);

	buttonRoot					= nullptr;

	eventButtonClick 				 += EventHandler(DatePickerPopup::DatePickerButtonClick);
	datePopupMenu.eventButtonOneClick += EventHandler(DatePickerPopup::DatePickerPopupOkButtonClick);
	datePopupMenu.eventButtonTwoClick += EventHandler(DatePickerPopup::DatePickerPopupCancelButtonClick);
	datePopupMenu.eventDateChanged  += EventHandler(DatePickerPopup::OnDateChangedCb);

}

std::tm const& TFC::Components::DatePickerPopup::GetSelectedDate() const {
	return selectedDate;
}

std::string const& TFC::Components::DatePickerPopup::GetFormatedSelectedDate() const {
	return formatedDate;
}

void TFC::Components::DatePickerPopup::SetSelectedDate(std::tm const& date) {
	datePopupMenu.SetSelectedDate(date);
}

void TFC::Components::DatePickerPopup::SetFormatedSelectedDate(std::string const& date) {
	datePopupMenu.SetFormatedSelectedDate(date);
	//formatedDate = datePopupMenu.GetFormatedSelectedDate();
	//selectedDate = datePopupMenu.GetSelectedDate();
}

void TFC::Components::DatePickerPopup::UseClassicTheme(bool const& classic) {
	datePopupMenu.UseClassicTheme(classic);
}

void TFC::Components::DatePickerPopup::SetOrientation(Elm_Popup_Orient const& orientation) {
	datePopupMenu.Orientation = orientation;
}

Elm_Popup_Orient const& TFC::Components::DatePickerPopup::GetOrientation() const {
	return datePopupMenu.Orientation;
}

void TFC::Components::DatePickerPopup::SetHint(std::string const& text) {
	this->buttonText = text;
}

std::string const& TFC::Components::DatePickerPopup::GetHint() const {
	return buttonText;
}
