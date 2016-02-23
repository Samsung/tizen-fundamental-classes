/*
 * SidebarView.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: gilang
 */



#include "SRIN/Components/SidebarView.h"

using namespace SRIN::Components;

LIBAPI SidebarView::SidebarView() :
	Framework::ViewBase("SidebarView")
{
	layout = leftPanel = background = currentContent = nullptr;
}

LIBAPI Evas_Object* SidebarView::CreateView(Evas_Object* root)
{
	layout = elm_layout_add(root);
	elm_layout_theme_set(layout, "layout", "drawer", "panel");

	leftPanel = elm_panel_add(layout);
	elm_panel_scrollable_set(leftPanel, EINA_TRUE);
	elm_panel_orient_set(leftPanel, ELM_PANEL_ORIENT_LEFT);
	elm_panel_hidden_set(leftPanel, EINA_TRUE);

	auto sidebarContent = CreateSidebar(leftPanel);

	elm_object_content_set(leftPanel, sidebarContent);
}

void SidebarView::Attach(ViewBase* view)
{
	if(currentContent != nullptr)
		Detach();

	currentContent = view->Create(layout);
	elm_object_part_content_set(layout, "elm.swallow.content", currentContent);
}

void SidebarView::Detach()
{
	elm_object_part_content_unset(layout, "elm.swallow.content");
	currentContent = nullptr;
}

Evas_Object* SidebarView::GetTitleLeftButton()
{
	auto btn = elm_button_add(layout);
	elm_object_style_set(btn, "naviframe/drawers");
	evas_object_smart_callback_add(btn, "clicked", SmartEventHandler, &drawerButtonClick);
}

Evas_Object* SidebarView::GetTitleRightButton()
{
	return nullptr;
}

CString SRIN::Components::SidebarView::GetContentStyle()
{
	return "drawers";
}
