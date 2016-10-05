/*
 * ServiceApplication.cpp
 *
 *  Created on: Sep 9, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Framework/ServiceApplication.h"

#include <system_settings.h>

#define InlineApplicationEventHandler(HANDLERNAME) [] (app_event_info_h e, void* a) { reinterpret_cast<ServiceApplicationBase*>(a)-> HANDLERNAME (e); }

void SRIN::Framework::ServiceApplicationBase::ApplicationTerminate() {
}

void SRIN::Framework::ServiceApplicationBase::ApplicationControl(
		app_control_h app_control) {
}

void SRIN::Framework::ServiceApplicationBase::LanguageChanged(
		app_event_info_h event_info, const char* locale) {
}

void SRIN::Framework::ServiceApplicationBase::OrientationChanged(
		app_event_info_h event_info) {
}

void SRIN::Framework::ServiceApplicationBase::RegionChanged(
		app_event_info_h event_info) {
}

void SRIN::Framework::ServiceApplicationBase::LowBattery(
		app_event_info_h event_info) {
}

void SRIN::Framework::ServiceApplicationBase::LowMemory(
		app_event_info_h event_info) {
}

SRIN::Framework::ServiceApplicationBase::ServiceApplicationBase(
		CString packageName) {
}

SRIN::Framework::ServiceApplicationBase::~ServiceApplicationBase() {
}



int SRIN::Framework::ServiceApplicationBase::Main(
		ServiceApplicationBase* appObj, int argc, char* argv[]) {

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

	return service_app_main(argc, argv, &event_callback, appObj);
}
