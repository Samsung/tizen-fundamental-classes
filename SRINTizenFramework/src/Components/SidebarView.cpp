/*
 * SidebarView.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Components/SidebarView.h"

using namespace SRIN::Components;

LIBAPI SidebarView::SidebarView() :
	Framework::ViewBase(),
	bg(nullptr)
{
	layout = leftPanel = background = currentContent = nullptr;
	drawerButtonClick += { this, &SidebarView::OnDrawerButtonClick };
	drawerScroll += { this, &SidebarView::OnDrawerScrolling };
}

LIBAPI Evas_Object* SidebarView::CreateView(Evas_Object* root)
{
	// Create base layout
	layout = elm_layout_add(root);
	elm_layout_theme_set(layout, "layout", "drawer", "panel");

	// Create panel layout for sidebar
	leftPanel = elm_panel_add(layout);
	elm_panel_scrollable_set(leftPanel, EINA_TRUE);
	elm_panel_orient_set(leftPanel, ELM_PANEL_ORIENT_LEFT);
	elm_panel_hidden_set(leftPanel, EINA_TRUE);
	evas_object_smart_callback_add(leftPanel, "scroll", SmartEventHandler, &drawerScroll);

	bg = evas_object_rectangle_add(evas_object_evas_get(layout));
	evas_object_color_set(bg, 0, 0, 0, 0);
	evas_object_show(bg);
	elm_object_part_content_set(layout, "elm.swallow.bg", bg);

	// Create sidebar content from subclass
	auto sidebarContent = CreateSidebar(leftPanel);
	elm_object_content_set(leftPanel, sidebarContent);

	// Set the panel to base layout
	elm_object_part_content_set(layout, "elm.swallow.left", leftPanel);

	return layout;
}

LIBAPI void SidebarView::Attach(ViewBase* view)
{
	if (currentContent != nullptr)
		Detach();

	currentContent = view->Create(layout);
	elm_object_part_content_set(layout, "elm.swallow.content", currentContent);
}

LIBAPI void SidebarView::Detach()
{
	elm_object_part_content_unset(layout, "elm.swallow.content");
	evas_object_hide(currentContent);
	currentContent = nullptr;
}

LIBAPI Evas_Object* SidebarView::GetTitleLeftButton(CString* buttonPart)
{
	auto btn = elm_button_add(layout);
	evas_object_smart_callback_add(btn, "clicked", SmartEventHandler, &drawerButtonClick);

	DrawerButtonStyle(btn);
	//*buttonPart = "drawers";
	return btn;
}

LIBAPI void SidebarView::OnDrawerButtonClick(ElementaryEvent* eventSource, Evas_Object* objSource, void* eventData)
{
	ToggleSidebar();
}

LIBAPI void SidebarView::OnDrawerScrolling(ElementaryEvent* eventSource, Evas_Object* objSource, void* eventData)
{
	auto ev = reinterpret_cast<Elm_Panel_Scroll_Info*>(eventData);
	int col = 127 * ev->rel_x;

	/* Change color for background dim */
	evas_object_color_set(bg, 0, 0, 0, col);
}

Evas_Object* SidebarView::GetTitleRightButton(CString* buttonPart)
{
	return nullptr;
}

LIBAPI CString SRIN::Components::SidebarView::GetContentStyle()
{
	return nullptr; //"drawers";
}

void SRIN::Components::SidebarView::DrawerButtonStyle(Evas_Object* button)
{
	elm_object_style_set(button, "naviframe/drawers");
}

void SRIN::Components::SidebarView::ToggleSidebar()
{
	if (!elm_object_disabled_get(leftPanel))
		elm_panel_toggle(leftPanel);
}
