/*
 * TabbarViewController.cpp
 *
 *  Created on: May 3, 2016
 *      Author: gilang
 */

#include "SRIN/Components/TabbarViewController.h"

#define FILE_EDC_CONTENT "SRIN/edc/Content.edj"


LIBAPI Evas_Object* SRIN::Components::TabbarViewController::CreateView(
		Evas_Object* root) {

	auto layout = elm_layout_add(root);
	elm_layout_theme_set(layout, "layout", "application", "toolbar-content");

	evas_object_event_callback_add(layout, EVAS_CALLBACK_RESIZE, EFL::EvasObjectEventHandler, &eventLayoutResize);

	auto itemLayout = elm_object_item_part_content_get(layout, NULL);
	auto obj = edje_object_part_object_get(itemLayout, "tabbar_top_divider");
	evas_object_color_set(const_cast<Evas_Object*>(obj), 100, 200, 100, 255);

	auto edjeLayout = elm_layout_edje_get(layout);
	tabbar = edje_object_part_swallow_get(edjeLayout, "elm.external.toolbar");
	elm_toolbar_shrink_mode_set(tabbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_select_mode_set(tabbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_toolbar_transverse_expanded_set(tabbar, EINA_TRUE);
	evas_object_show(tabbar);

	// Scroller
	this->scroller = elm_scroller_add(layout);
	elm_scroller_loop_set(this->scroller, EINA_FALSE, EINA_FALSE);
	evas_object_size_hint_weight_set(this->scroller, 0.5, 0.5);
	evas_object_size_hint_align_set(this->scroller, 0.5, 0.5);
	elm_scroller_page_relative_set(this->scroller, 1.0, 0.0);
	elm_scroller_policy_set(this->scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	elm_scroller_page_scroll_limit_set(this->scroller, 1, 0);
	elm_object_scroll_lock_y_set(this->scroller, EINA_TRUE);
	evas_object_size_hint_weight_set(this->scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(this->scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(this->scroller);
	evas_object_smart_callback_add(this->scroller, "scroll,page,changed", EFL::EvasSmartEventHandler, &this->eventTabContentScrolled);

	elm_object_text_set(layout, "Test");



	elm_object_part_content_set(layout, "elm.swallow.content", this->scroller);

	// Box inside scroller
	this->box = elm_box_add(this->scroller);
	elm_box_horizontal_set(this->box, EINA_TRUE);
	elm_object_content_set(this->scroller, this->box);
	evas_object_size_hint_weight_set(this->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(this->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(this->box);
	elm_box_homogeneous_set(this->box, EINA_TRUE);


	int num = 0;


	auto edj_path = Framework::ApplicationBase::GetResourcePath(FILE_EDC_CONTENT);

	for(TabEntry& tab : this->tabs)
	{
		auto content = edje_object_add(evas_object_evas_get(this->box));
		edje_object_file_set(content, edj_path.c_str(), "main");

		auto viewRoot = tab.controller->View->Create(this->box);
		edje_object_part_swallow(content, "elm.swallow.content", viewRoot);
		evas_object_show(content);
		elm_box_pack_end(this->box, content);

		tab.objectItem = elm_toolbar_item_append(tabbar, nullptr, tab.tabText.length() ? tab.tabText.c_str() : nullptr, EFL::EvasSmartEventHandler, &this->eventTabbarButtonClicked);
		tab.tabNumber = num;
		tab.content = content;

		if(num == 0)
			tab.controller->Reload(nullptr);
		num++;
	}

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(layout);

	return layout;
}

LIBAPI SRIN::Components::TabbarViewController::TabbarViewController(SRIN::Framework::ControllerManager* m, CString controllerName) :
		SRIN::Framework::ControllerBase(m, this, controllerName),
		box(nullptr), scroller(nullptr)
{
	this->disableChangeTabByScroll = false;
	this->currentTab = 0;
	this->eventLayoutResize += EventHandler(TabbarViewController::OnLayoutResize);
	this->eventTabbarButtonClicked += EventHandler(TabbarViewController::OnTabbarButtonClicked);
	this->eventTabContentScrolled += EventHandler(TabbarViewController::OnTabContentScrolled);
}

void SRIN::Components::TabbarViewController::OnLayoutResize(
		EFL::EvasObjectEvent* event, EFL::EvasEventSourceInfo* objSource, void* event_data) {
	Evas_Coord w, h;

	dlog_print(DLOG_DEBUG, LOG_TAG, "Tabbar Layout Resize");

	evas_object_geometry_get(objSource->obj, NULL, NULL, &w, &h);

	Evas_Coord tabW, tabH;
	evas_object_geometry_get(this->tabbar, NULL, NULL, &tabW, &tabH);

	for(TabEntry& tab : this->tabs)
	{
		evas_object_size_hint_min_set(tab.content, w, h - tabH);
		evas_object_size_hint_max_set(tab.content, w, h - tabH);
	}

	elm_scroller_page_size_set(this->scroller, w, h);

	//elm_scroller_page_show(this->scroller, this->current_page, 0);
}

LIBAPI void SRIN::Components::TabbarViewController::AddTab(
		std::string text,
		Framework::ControllerBase& controller) {
	TabEntry entry;
	entry.controller = &controller;
	entry.tabText = text;
	entry.objectItem = nullptr;
	this->tabs.push_back(entry);
}

void SRIN::Components::TabbarViewController::OnTabbarButtonClicked(
		EFL::EvasSmartEvent* event, Evas_Object* source, void* event_data) {
	auto item = reinterpret_cast<Elm_Object_Item*>(event_data);

	this->LookupAndBringContent(item);

	//elm_scroller_page_show(this->scroller, tabItem->tabNumber, 0);

	dlog_print(DLOG_DEBUG, LOG_TAG, "Tabbar button click %d %d", source, event_data);
}

void SRIN::Components::TabbarViewController::LookupAndBringContent(
		Elm_Object_Item* tabItem) {

	for(TabEntry& tab : this->tabs)
	{
		if(tab.objectItem == tabItem)
		{


			if(this->currentTab == tab.tabNumber)
				break;



			this->currentTab = tab.tabNumber;
			this->disableChangeTabByScroll = true;
			elm_scroller_page_bring_in(this->scroller, tab.tabNumber, 0);
			tab.controller->Reload(nullptr);
			break;
		}
	}
}

void SRIN::Components::TabbarViewController::OnTabContentScrolled(
		EFL::EvasSmartEvent* event, Evas_Object* source, void* event_data) {



#if _DEBUG
	static int count = 0;
	dlog_print(DLOG_DEBUG, LOG_TAG, "Tab content scrolled: %d", count);
	count++;
#endif

	if(this->disableChangeTabByScroll)
	{
#if _DEBUG
		dlog_print(DLOG_DEBUG, LOG_TAG, "Tab content scrolled event DISCARDED");
#endif
		this->disableChangeTabByScroll = false;
		return;
	}

	int pageH, pageV;
	elm_scroller_current_page_get(source, &pageH, &pageV);

	if(this->currentTab == pageH)
		return;

	this->currentTab = pageH;

	auto objectItem = this->tabs[pageH].objectItem;

	if(elm_toolbar_item_selected_get(objectItem) != EINA_TRUE)
		elm_toolbar_item_selected_set(objectItem, EINA_TRUE);

	this->tabs[pageH].controller->Reload(nullptr);
}
