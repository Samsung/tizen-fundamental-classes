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
 *    Framework/ServiceApplication.cpp
 *
 * Created on:  Sep 9, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Framework/ServiceApplication.h"
#include <dlog.h>
#include <system_settings.h>
#include <dlog.h>
#define InlineApplicationEventHandler(HANDLERNAME) [] (app_event_info_h e, void* a) { reinterpret_cast<ServiceApplicationBase*>(a)-> HANDLERNAME (e); }

void TFC::Framework::ServiceApplicationBase::ApplicationTerminate() {
}

void TFC::Framework::ServiceApplicationBase::ApplicationControl(
		app_control_h app_control) {
}

void TFC::Framework::ServiceApplicationBase::LanguageChanged(
		app_event_info_h event_info, const char* locale) {
}

void TFC::Framework::ServiceApplicationBase::OrientationChanged(
		app_event_info_h event_info) {

}

void TFC::Framework::ServiceApplicationBase::RegionChanged(
		app_event_info_h event_info) {
}

void TFC::Framework::ServiceApplicationBase::LowBattery(
		app_event_info_h event_info) {
}

void TFC::Framework::ServiceApplicationBase::LowMemory(
		app_event_info_h event_info) {
}

TFC::Framework::ServiceApplicationBase::ServiceApplicationBase(
		char const* packageName) {
}

TFC::Framework::ServiceApplicationBase::~ServiceApplicationBase() {
}



int TFC::Framework::ServiceApplicationBase::Main(
		ServiceApplicationBase* appObj, int argc, char* argv[])
try
{

	service_app_lifecycle_callback_s event_callback;
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = [] (void* a) { return reinterpret_cast<ServiceApplicationBase*>(a)->ApplicationCreate(); };
	event_callback.terminate = [] (void* a) { reinterpret_cast<ServiceApplicationBase*>(a)->ApplicationTerminate(); };
	event_callback.app_control = [] (app_control_h app_control, void* a) { reinterpret_cast<ServiceApplicationBase*>(a)->ApplicationControl(app_control); };

	auto languageChangedHandler = [] (app_event_info_h e, void* a) {
		char *locale = NULL;
		system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
		ServiceApplicationBase* app = static_cast<ServiceApplicationBase*>(a);
		app->LanguageChanged(e, locale);

		free(locale);
	};

	service_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, InlineApplicationEventHandler(LowBattery), appObj);
	service_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, InlineApplicationEventHandler(LowMemory), appObj);
	service_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, languageChangedHandler, appObj);
	service_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, InlineApplicationEventHandler(RegionChanged), appObj);

	auto retCode = service_app_main(argc, argv, &event_callback, appObj);

	switch(retCode)
	{
	case APP_ERROR_INVALID_PARAMETER:
		dlog_print(DLOG_DEBUG, LOG_TAG, "Failed to launch service application: Invalid parameter");
		break;
	case APP_ERROR_INVALID_CONTEXT:
		dlog_print(DLOG_DEBUG, LOG_TAG, "Failed to launch service application: Invalid context");
		break;
	case APP_ERROR_ALREADY_RUNNING:
		dlog_print(DLOG_DEBUG, LOG_TAG, "Failed to launch service application: Already running");
		break;
	}

	return retCode;
}
catch(TFC::TFCException& ex)
{
	dlog_print(DLOG_ERROR, argv[0], "Exception occured (%s) => %s; Stack trace %s", typeid(ex).name(), ex.what(), ex.GetStackTrace().c_str());
	throw;
}
