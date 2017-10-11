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
 *    Components/Toast.cpp
 *
 * Created on:  Mar 16, 2016
 * Author: 		Ida Bagus Putu Peradnya Dinata (ib.putu@samsung.com)
 * Contributor: Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Components/Toast.h"
#include <string>

TFC::Components::Toast toastObj;

LIBAPI bool TFC::Components::Toast::BackButtonPressed(Evas_Object* objSource, void* eventData) {
	OnDismiss(objSource, eventData);
	return false;
}

LIBAPI TFC::Components::Toast::Toast() {
	this->eventDismiss += EventHandler(Toast::OnDismiss);
	this->eventTimeout += EventHandler(Toast::OnDismiss);
}


LIBAPI void TFC::Components::Toast::Show(const std::string& message) {
	Show(message, 2.0);
}

LIBAPI void TFC::Components::Toast::OnDismiss(Evas_Object* objSource, void* eventData)
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
	try
	{
		toastObj.eventDismiss.Bind(popup, "block,clicked");
		toastObj.eventTimeout.Bind(popup, "timeout");

		evas_object_show(popup);
	}
	catch(TFC::TFCException const& ex)
	{
		evas_object_del(popup); // For now cancel
	}
}

