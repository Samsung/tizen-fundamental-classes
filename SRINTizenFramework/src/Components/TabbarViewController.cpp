/*
 * TabbarViewController.cpp
 *
 *  Created on: May 3, 2016
 *      Author: gilang
 */

#include "SRIN/Components/TabbarViewController.h"


LIBAPI Evas_Object* SRIN::Components::TabbarViewController::CreateView(
		Evas_Object* root) {

	auto layout = elm_layout_add(root);
	elm_layout_theme_set(layout, "layout", "application", "toolbar-content");

	auto itemLayout = elm_object_item_part_content_get(layout, NULL);
	auto obj = edje_object_part_object_get(itemLayout, "tabbar_top_divider");
	evas_object_color_set(const_cast<Evas_Object*>(obj), 100, 200, 100, 255);


	auto edjeLayout = elm_layout_edje_get(layout);

	auto tabbar = edje_object_part_swallow_get(edjeLayout, "elm.external.toolbar");
	elm_toolbar_shrink_mode_set(tabbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_transverse_expanded_set(tabbar, EINA_TRUE);

	/*
	for(int i = 0; i < itemCount; i++) {
		TabItem* item = tabItems[i];
		TabbarCallbackPackage* package = new TabbarCallbackPackage();
		package->view = this;
		package->item = item;

		item->initialized = true;
		item->itemHandle = elm_toolbar_item_append(tabbar, item->itemIcon->c_str(), nullptr, TabbarView_ItemCallback, package);
		elm_object_item_del_cb_set(item->itemHandle, TabbarView_ItemCleanup);
	}
	*/


	elm_toolbar_item_append(tabbar, nullptr, "Tab 1", nullptr, nullptr);
	elm_toolbar_item_append(tabbar, nullptr, "Tab 2", nullptr, nullptr);
	evas_object_show(tabbar);
	elm_object_content_set(layout, tabbar);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(layout);

	return layout;
}

LIBAPI SRIN::Components::TabbarViewController::TabbarViewController(SRIN::Framework::ControllerManager* m, CString controllerName) :
		SRIN::Framework::ControllerBase(m, this, controllerName)
{
}
