/*
 * Toast.cpp
 *
 *  Created on: Mar 16, 2016
 *      Contributor:
 *        ib.putu (ib.putu@samsung.com)
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Components/Toast.h"
#include <string>

TFC::Components::Toast toastObj;

LIBAPI bool TFC::Components::Toast::BackButtonPressed(EFL::EvasSmartEvent* event, Evas_Object* objSource, void* eventData) {
	OnDismiss(event, objSource, eventData);
	return false;
}

LIBAPI TFC::Components::Toast::Toast() {
	this->eventDismiss += EventHandler(Toast::OnDismiss);
}


LIBAPI void TFC::Components::Toast::Show(const std::string& message) {
	Show(message, 2.0);
}

LIBAPI void TFC::Components::Toast::OnDismiss(EFL::EvasSmartEvent* event, Evas_Object* objSource, void* eventData)
{
	evas_object_del(objSource);
}

LIBAPI void TFC::Components::Toast::Show(const std::string& message, double timeout) {
	auto popup = elm_popup_add(TFC::Framework::UIApplicationBase::CurrentInstance->GetApplicationConformant());
	elm_object_style_set(popup, "toast");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if(message.size() > 0)
		elm_object_text_set(popup, message.c_str());

	evas_object_smart_callback_add(popup, "block,clicked", EFL::EvasSmartEventHandler, &toastObj.eventDismiss);

	elm_popup_timeout_set(popup, timeout);
	evas_object_smart_callback_add(popup, "timeout", EFL::EvasSmartEventHandler, &toastObj.eventDismiss);
	evas_object_show(popup);
}

