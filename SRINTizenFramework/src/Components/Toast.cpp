/*
 * Toast.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: Ida Bagus Putu Peradnya Dinata
 */

#include "SRIN/Components/Toast.h"
#include <string>

LIBAPI void SRIN::Components::Toast::OnDismissEnded(Framework::ViewBase* viewSource,
		Evas_Object* objSource, void* eventData) {
	Dismiss();
}

LIBAPI Evas_Object* SRIN::Components::Toast::CreateComponent(Evas_Object* root) {
	return nullptr;
}

LIBAPI Evas_Object* SRIN::Components::Toast::CreateContent(Evas_Object* root) {
	return nullptr;
}

LIBAPI bool SRIN::Components::Toast::BackButtonPressed() {
	Dismiss();
	return false;
}

LIBAPI SRIN::Components::Toast::Toast() {
	this->popup 	= nullptr;
	this->Visible 	= false;
	this->message 	= "";
}

LIBAPI void SRIN::Components::Toast::SetMessage(const std::string& text) {
	message = text;
}

LIBAPI std::string& SRIN::Components::Toast::GetMessageString() {
	return message;
}

LIBAPI void SRIN::Components::Toast::Show() {
	Show(2.0);
}

LIBAPI void SRIN::Components::Toast::Show(double timeout) {
	this->popup = elm_popup_add(SRIN::Framework::ApplicationBase::CurrentInstance->GetApplicationConformant());
	elm_object_style_set(popup, "toast");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if(message.size() > 0)
		elm_object_text_set(popup, message.c_str());

	evas_object_smart_callback_add(popup, "block,clicked", SmartEventHandler, &onBackButtonPressed);

	elm_popup_timeout_set(popup, timeout);
	evas_object_smart_callback_add(popup, "timeout", SmartEventHandler, &onBackButtonPressed);
	evas_object_show(this->popup);
}

LIBAPI void SRIN::Components::Toast::Dismiss() {
	evas_object_del(this->popup);
	this->popup = nullptr;
}
