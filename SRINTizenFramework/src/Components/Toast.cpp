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

LIBAPI bool TFC::Components::Toast::BackButtonPressed(EvasSmartEvent::Type* event, Evas_Object* objSource, void* eventData) {
	OnDismiss(event, objSource, eventData);
	return false;
}

LIBAPI TFC::Components::Toast::Toast() {
	this->eventDismiss += EventHandler(Toast::OnDismiss);
}


LIBAPI void TFC::Components::Toast::Show(const std::string& message) {
	Show(message, 2.0);
}

LIBAPI void TFC::Components::Toast::OnDismiss(EvasSmartEvent::Type* event, Evas_Object* objSource, void* eventData)
{
	evas_object_del(objSource);
}

LIBAPI void TFC::Components::Toast::Show(const std::string& message, double timeout) {
	auto uiApplicationBase = dynamic_cast<Framework::UIApplicationBase*>(Framework::UIApplicationBase::GetCurrentInstance());
	auto popup = elm_popup_add(uiApplicationBase->GetApplicationConformant());

	elm_object_style_set(popup, "toast");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if(message.size() > 0)
		elm_object_text_set(popup, message.c_str());


	elm_popup_timeout_set(popup, timeout);

	//evas_object_smart_callback_add(popup, "block,clicked", EvasSmartEventHandler, &toastObj.eventDismiss);
	//evas_object_smart_callback_add(popup, "timeout", EvasSmartEventHandler, &toastObj.eventDismiss);

	toastObj.eventDismiss.Bind(popup, "block,clicked");
	toastObj.eventDismiss.Bind(popup, "timeout");

	evas_object_show(popup);
}

