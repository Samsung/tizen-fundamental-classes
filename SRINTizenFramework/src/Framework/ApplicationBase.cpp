/*
 * ApplicationBase.cpp
 *
 * Source file for class ApplicationBase
 *
 *  Created on: Feb 12, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

bool ApplicationBase_AppCreateHandler(void *data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	return app->ApplicationCreate();
}

void ApplicationBase_AppControlHandler(app_control_h app_control, void* data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->ApplicationControl(app_control);
}

void ApplicationBase_AppPauseHandler(void* data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->ApplicationPause();
}

void ApplicationBase_AppResumeHandler(void* data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->ApplicationResume();
}

void ApplicationBase_AppTerminateHandler(void* data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->ApplicationTerminate();
}

void ui_app_lang_changed(app_event_info_h event_info, void* data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);

	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->LanguageChanged(event_info, locale);

	free(locale);
	return;
}

void ui_app_orient_changed(app_event_info_h event_info, void *data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->OrientationChanged(event_info);
}

void ui_app_region_changed(app_event_info_h event_info, void *data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->RegionChanged(event_info);
}

void ui_app_low_battery(app_event_info_h event_info, void *data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->LowBattery(event_info);
}

void ui_app_low_memory(app_event_info_h event_info, void *data)
{
	ApplicationBase* app = static_cast<ApplicationBase*>(data);
	app->LowMemory(event_info);
}

void win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ApplicationBase* app = (ApplicationBase*) data;
	dlog_print(DLOG_DEBUG, LOG_TAG, "Back button pressed");
	app->BackButtonPressed();
}

SimpleReadOnlyProperty<ApplicationBase, ApplicationBase*> ApplicationBase::CurrentInstance;
SimpleReadOnlyProperty<ApplicationBase, CString> ApplicationBase::ResourcePath;

int ApplicationBase::Main(ApplicationBase* app, int argc, char* argv[])
{
	ApplicationBase::CurrentInstance = app;
	ApplicationBase::ResourcePath = app_get_resource_path();

	::ui_app_lifecycle_callback_s event_callback =
	{ ApplicationBase_AppCreateHandler, ApplicationBase_AppTerminateHandler, ApplicationBase_AppPauseHandler,
		ApplicationBase_AppResumeHandler, ApplicationBase_AppControlHandler };

	::app_event_handler_h handlers[5] =
	{ NULL, };

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, app);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, app);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED,
		ui_app_orient_changed, app);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed,
		app);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED,
		ui_app_region_changed, app);

	int ret = ::ui_app_main(argc, argv, &event_callback, app);
	if (ret != APP_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}

LIBAPI ApplicationBase::ApplicationBase(CString packageName) :
	packageName(packageName)
{
	this->rootFrame = this->win = this->conform = NULL;
	this->backButtonCallback = nullptr;
	this->backButtonInstance = nullptr;
	this->haveEventBackPressed = false;

}

LIBAPI void ApplicationBase::ApplicationControl(app_control_h app_control)
{
}

LIBAPI void ApplicationBase::ApplicationPause()
{
}

LIBAPI void ApplicationBase::ApplicationResume()
{
}

LIBAPI void ApplicationBase::ApplicationTerminate()
{
}

LIBAPI void ApplicationBase::LanguageChanged(app_event_info_h event_info, const char* locale)
{
}

LIBAPI void ApplicationBase::OrientationChanged(app_event_info_h event_info)
{
}

LIBAPI void ApplicationBase::RegionChanged(app_event_info_h event_info)
{
}

LIBAPI void ApplicationBase::LowBattery(app_event_info_h event_info)
{
}

LIBAPI void ApplicationBase::LowMemory(app_event_info_h event_info)
{
}

LIBAPI bool ApplicationBase::OnBackButtonPressed()
{
	return true;
}

LIBAPI void ApplicationBase::EnableBackButtonCallback(bool enable)
{
	if (enable)
	{
		if (!this->haveEventBackPressed)
		{
			eext_object_event_callback_add(this->win, EEXT_CALLBACK_BACK, win_delete_request_cb, this);
			this->haveEventBackPressed = true;
		}
	}
	else
	{
		eext_object_event_callback_del(this->win, EEXT_CALLBACK_BACK, win_delete_request_cb);
		this->haveEventBackPressed = false;
	}
}

LIBAPI bool ApplicationBase::AcquireExclusiveBackButtonPressed(EventClass* instance, BackButtonCallback callback)
{
}

LIBAPI bool ApplicationBase::ReleaseExclusiveBackButtonPressed(EventClass* instance, BackButtonCallback callback)
{
}

LIBAPI void ApplicationBase::Attach(ViewBase* view)
{
	Evas_Object* viewComponent = view->Create(this->rootFrame);

	//show to window
	if (viewComponent != NULL)
	{
		auto naviframeContent = dynamic_cast<INaviframeContent*>(view);

		CString naviframeStyle = nullptr;

		if (naviframeContent)
			naviframeStyle = naviframeContent->GetContentStyle();

		auto naviframeItem = elm_naviframe_item_push(this->rootFrame, view->Title->c_str(), NULL, NULL,
			viewComponent, naviframeStyle);

		auto backButton = elm_object_item_part_content_get(naviframeItem, "elm.swallow.prev_btn");
		auto style = elm_object_style_get(backButton);

		// Title button handling

		if (naviframeContent)
		{
			auto left = naviframeContent->GetTitleLeftButton();
			if (left)
			{
				auto oldObj = elm_object_item_part_content_unset(naviframeItem, "title_left_btn");
				evas_object_hide(oldObj);
				elm_object_item_part_content_set(naviframeItem, "title_left_btn", left);
				evas_object_show(left);
			}

			auto right = naviframeContent->GetTitleRightButton();
			if (right)
			{
				auto oldObj = elm_object_item_part_content_unset(naviframeItem, "title_right_btn");
				evas_object_hide(oldObj);
				elm_object_item_part_content_set(naviframeItem, "title_right_btn", right);
				evas_object_show(right);
			}

			naviframeContent->RaiseAfterNaviframePush(naviframeItem);
		}



		dlog_print(DLOG_DEBUG, LOG_TAG, "Prev Button Style: %s", style);

		evas_object_smart_callback_add(backButton, "clicked", [] (void* a, Evas_Object* b, void* c)
		{	static_cast<ApplicationBase*>(a)->BackButtonPressed();}, this);

		evas_object_show(viewComponent);
	}
}

LIBAPI void ApplicationBase::Detach()
{
	elm_naviframe_item_pop(this->rootFrame);
}

LIBAPI void ApplicationBase::Exit()
{
}

LIBAPI void ApplicationBase::BackButtonPressed()
{
	bool backResult = backButtonCallback ? (backButtonInstance->*backButtonCallback)() : OnBackButtonPressed();
	if (backResult)
		ui_app_exit();
}

LIBAPI Evas_Object* ApplicationBase::GetApplicationConformant()
{
	return this->conform;
}

LIBAPI bool ApplicationBase::ApplicationCreate()
{
	elm_config_accel_preference_set("3d");

	// Window
	// Create and initialize elm_win.
	// elm_win is mandatory to manipulate window.
	this->win = elm_win_util_standard_add(packageName, packageName);

	elm_win_autodel_set(this->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(this->win))
	{
		int rots[1] =
		{ 0 };
		elm_win_wm_rotation_available_rotations_set(this->win, (const int *) (&rots), 1);
	}

	//eext_object_event_callback_add(this->win, EEXT_CALLBACK_BACK, win_delete_request_cb, (void*)this);

	//evas_object_smart_callback_add(this->win, "delete,request", win_delete_request_cb, NULL);
	//eext_object_event_callback_add(this->win, EEXT_CALLBACK_BACK, win_delete_request_cb, this);
	EnableBackButtonCallback(true);
	// Conformant
	// Create and initialize elm_conformant.
	// elm_conformant is mandatory for base gui to have proper size
	// when indicator or virtual keypad is visible.
	this->conform = elm_conformant_add(this->win);
	elm_win_indicator_mode_set(this->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(this->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(this->conform, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	elm_win_resize_object_add(this->win, this->conform);
	evas_object_show(this->conform);

	// Naviframe
	this->rootFrame = elm_naviframe_add(this->conform);

	elm_naviframe_prev_btn_auto_pushed_set(this->rootFrame, EINA_TRUE);
	elm_naviframe_content_preserve_on_pop_set(this->rootFrame, EINA_FALSE);
	elm_object_content_set(this->conform, this->rootFrame);
	evas_object_show(this->rootFrame);

	evas_object_show(this->win);

	this->OnApplicationCreated();
	return true;
}

LIBAPI void ApplicationBase::OnApplicationCreated()
{

}

LIBAPI ApplicationBase::~ApplicationBase()
{
}

void SRIN::Framework::ApplicationBase::SetIndicatorVisibility(bool value)
{
	if (value)
	{
		elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_SHOW);
	}
	else
	{
		elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_HIDE);
	}
}

std::string SRIN::Framework::ApplicationBase::GetResourcePath(CString path)
{
	std::string ret = CString(ApplicationBase::ResourcePath);
	ret += path;
	return ret;
}

LIBAPI void SRIN::Framework::ApplicationBase::SetIndicatorColor(Color color)
{
	Evas_Object* bg = elm_object_part_content_get(this->conform, "elm.swallow.indicator_bg");

	if(bg == nullptr)
	{
		bg = evas_object_rectangle_add(evas_object_evas_get(this->win));
		elm_object_part_content_set(this->conform, "elm.swallow.indicator_bg", bg);
	}

	evas_object_color_set(bg, color.r, color.g, color.b, color.a);

	elm_win_indicator_opacity_set(win, ELM_WIN_INDICATOR_OPAQUE);
}

LIBAPI void SRIN::Framework::IndicatorStyler::OnPostNavigation(Event<ControllerManager*, ControllerBase*>* event,
	ControllerManager* manager, ControllerBase* controller)
{
	auto colorable = dynamic_cast<IIndicatorColor*>(controller->View);
	if(colorable)
		app->SetIndicatorColor(colorable->IndicatorColor);
	else
		app->SetIndicatorColor(defaultColor);
}

LIBAPI SRIN::Framework::IndicatorStyler::IndicatorStyler(ApplicationBase* app, ControllerManager* manager, Color defaultColor) :
	app(app), manager(manager), defaultColor(defaultColor)
{
	manager->NavigationProcessed += { this, &IndicatorStyler::OnPostNavigation };
}

LIBAPI SRIN::Framework::IndicatorStyler::~IndicatorStyler()
{
	manager->NavigationProcessed -= { this, &IndicatorStyler::OnPostNavigation };
}

LIBAPI SRIN::Framework::INaviframeContent::INaviframeContent() :
	naviframeItem(nullptr)
{

}

LIBAPI void SRIN::Framework::INaviframeContent::RaiseAfterNaviframePush(Elm_Object_Item* naviframeItem)
{
	this->naviframeItem = naviframeItem;
	AfterNaviframePush(naviframeItem);
}

LIBAPI void SRIN::Framework::INaviframeContent::SetTitle(const std::string& value)
{
	ITitleProvider::SetTitle(value);
	elm_object_item_part_text_set(naviframeItem, "elm.text.title", value.c_str());
	dlog_print(DLOG_DEBUG, LOG_TAG, "Set title on INavCont %s", value.c_str());
}

SRIN::Framework::ITitleProvider::ITitleProvider() :
	Title(this)
{
}

SRIN::Framework::ITitleProvider::~ITitleProvider()
{
}
