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
 *    Components/TabbarViewController.cpp
 *
 * Created on:  May 3, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#include <dlog.h>
#include "TFC/Components/TabbarViewController.h"

#define FILE_EDC_CONTENT "TFC/edc/Content.edj"


LIBAPI Evas_Object* TFC::Components::TabbarViewController::CreateView(
		Evas_Object* root) {

	auto layout = elm_layout_add(root);
	elm_layout_theme_set(layout, "layout", "application", "toolbar-content");

	//evas_object_event_callback_add(layout, EVAS_CALLBACK_RESIZE, EFL::EvasObjectEventHandler, &eventLayoutResize);
	eventLayoutResize.Bind(layout, EVAS_CALLBACK_RESIZE);

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
	//evas_object_smart_callback_add(this->scroller, "scroll,page,changed", EFL::EvasSmartEventHandler, &this->eventTabContentScrolled);
	eventTabContentScrolled.Bind(this->scroller, "scroll,page,changed");

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

		//tab.objectItem = elm_toolbar_item_append(tabbar, nullptr, tab.tabText.length() ? tab.tabText.c_str() : nullptr, EFL::EvasSmartEventHandler, &this->eventTabbarButtonClicked);
		tab.objectItem = elm_toolbar_item_append(tabbar, nullptr, tab.tabText.length() ? tab.tabText.c_str() : nullptr, &TabbarViewController::OnTabbarButtonClicked, this);
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

LIBAPI TFC::Components::TabbarViewController::TabbarViewController(TFC::Framework::ControllerManager* m, char const* controllerName) :
		TFC::Framework::ControllerBase(m, this, controllerName),
		scroller(nullptr), box(nullptr), tabbar(nullptr)
{
	this->disableChangeTabByScroll = false;
	this->currentTab = 0;
	this->eventLayoutResize += EventHandler(TabbarViewController::OnLayoutResize);
	this->eventTabContentScrolled += EventHandler(TabbarViewController::OnTabContentScrolled);
	//this->eventTabbarButtonClicked += EventHandler(TabbarViewController::OnTabbarButtonClicked);
}

void TFC::Components::TabbarViewController::OnLayoutResize(EFL::EvasEventSourceInfo objSource, void* event_data)
{
	Evas_Coord w, h;

	dlog_print(DLOG_DEBUG, LOG_TAG, "Tabbar Layout Resize");

	evas_object_geometry_get(objSource.obj, NULL, NULL, &w, &h);

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

LIBAPI void TFC::Components::TabbarViewController::AddTab(
		std::string text,
		Framework::ControllerBase& controller) {
	TabEntry entry;
	entry.controller = &controller;
	entry.tabText = text;
	entry.objectItem = nullptr;
	this->tabs.push_back(entry);
}

/*
void TFC::Components::TabbarViewController::OnTabbarButtonClicked(
		EvasSmartEvent::Type* event, Evas_Object* source, void* event_data) {
	auto item = reinterpret_cast<Elm_Object_Item*>(event_data);

	this->LookupAndBringContent(item);

	//elm_scroller_page_show(this->scroller, tabItem->tabNumber, 0);

	dlog_print(DLOG_DEBUG, LOG_TAG, "Tabbar button click %d %d", source, event_data);
}
*/

void TFC::Components::TabbarViewController::OnTabbarButtonClicked(void *data, Evas_Object *source, void *event_data)
{
	auto tabbarVC = reinterpret_cast<TabbarViewController*>(data);
	auto item = reinterpret_cast<Elm_Object_Item*>(event_data);

	tabbarVC->LookupAndBringContent(item);

	//elm_scroller_page_show(this->scroller, tabItem->tabNumber, 0);

	dlog_print(DLOG_DEBUG, LOG_TAG, "Tabbar button click %d %d", source, event_data);
}

void TFC::Components::TabbarViewController::LookupAndBringContent(
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

void TFC::Components::TabbarViewController::OnTabContentScrolled(
		Evas_Object* source, void* event_data) {



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

void TFC::Components::TabbarViewController::OnReload(TFC::ObjectClass* param) {
	this->tabs[this->currentTab].controller->Reload(param);
}
