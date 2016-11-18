/*
 * SidebarView.cpp
 *
 *  Created on: Feb 22, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/SidebarView.h"
#include <limits>

using namespace TFC::Components;

LIBAPI SidebarView::SidebarView() :
	Framework::ViewBase(),
	bg(nullptr),
	contentWrapper(nullptr)
{
	layout = leftPanel = background = currentContent = nullptr;
	drawerButtonClick += { this, &SidebarView::OnDrawerButtonClick };
	drawerScroll += { this, &SidebarView::OnDrawerScrolling };

	DrawerOpened += { this, &SidebarView::OnDrawerOpened };
	DrawerClosed += { this, &SidebarView::OnDrawerClosed };
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
	//evas_object_smart_callback_add(leftPanel, "scroll", EFL::EvasSmartEventHandler, &drawerScroll);
	drawerScroll.Bind(leftPanel, "scroll");

	bg = evas_object_rectangle_add(evas_object_evas_get(layout));
	evas_object_color_set(bg, 0, 0, 0, 0);
	evas_object_show(bg);
	elm_object_part_content_set(layout, "elm.swallow.bg", bg);

	// Create sidebar content from subclass
	auto sidebarContent = CreateSidebar(root);
	elm_object_content_set(leftPanel, sidebarContent);

	// Create content wrapper if any
	this->contentWrapper = CreateContentWrapper(layout);
	if(this->contentWrapper != nullptr)
		elm_object_part_content_set(layout, "elm.swallow.content", contentWrapper);

	// Set the panel to base layout
	elm_object_part_content_set(layout, "elm.swallow.left", leftPanel);

	return layout;
}

LIBAPI void SidebarView::Attach(ViewBase* view)
{
	if (currentContent != nullptr)
		Detach();

	currentContent = view->Create(this->contentWrapper ? this->contentWrapper : layout);
	elm_object_part_content_set(this->contentWrapper ? this->contentWrapper : layout, "elm.swallow.content", currentContent);
}

LIBAPI void SidebarView::Detach()
{
	elm_object_part_content_unset(this->contentWrapper ? this->contentWrapper : layout, "elm.swallow.content");
	evas_object_hide(currentContent);
	currentContent = nullptr;
}

LIBAPI Evas_Object* SidebarView::GetTitleLeftButton(char const* buttonPart)
{
	auto btn = elm_button_add(layout);
	//evas_object_smart_callback_add(btn, "clicked", EFL::EvasSmartEventHandler, &drawerButtonClick);
	drawerButtonClick.Bind(btn, "clicked");

	DrawerButtonStyle(btn);
	//*buttonPart = "drawers";
	return btn;
}

LIBAPI void SidebarView::OnDrawerButtonClick(Evas_Object* objSource, void* eventData)
{
	ToggleSidebar();
}

LIBAPI void SidebarView::OnDrawerScrolling(Evas_Object* objSource, void* eventData)
{
	auto ev = reinterpret_cast<Elm_Panel_Scroll_Info*>(eventData);
	int col = 127 * ev->rel_x;

	/* Change color for background dim */
	evas_object_color_set(bg, 0, 0, 0, col);

	/* Trigger on open/closed event */
	double epsilon = std::numeric_limits<double>::epsilon();
	if (ev->rel_x >= 1.0 - epsilon) {
		DrawerOpened(this, nullptr);
	}
	else if (ev->rel_x <= 0.0 + epsilon) {
		DrawerClosed(this, nullptr);
	}
}

void SidebarView::OnDrawerOpened(SidebarView* sidebarView, void* unused)
{
	BackButtonHandler::Acquire();
}

void SidebarView::OnDrawerClosed(SidebarView* sidebarView, void* unused)
{
	BackButtonHandler::Release();
}

Evas_Object* SidebarView::GetTitleRightButton(char const* buttonPart)
{
	return nullptr;
}

LIBAPI char const* TFC::Components::SidebarView::GetContentStyle()
{
	return nullptr; //"drawers";
}

void TFC::Components::SidebarView::DrawerButtonStyle(Evas_Object* button)
{
	elm_object_style_set(button, "naviframe/drawers");
}

LIBAPI Evas_Object* TFC::Components::SidebarView::CreateContentWrapper(Evas_Object* root)
{
	return nullptr;
}

void TFC::Components::SidebarView::ToggleSidebar()
{
	if (!elm_object_disabled_get(leftPanel))
		elm_panel_toggle(leftPanel);
}

void TFC::Components::SidebarView::ToggleSidebar(bool show)
{
	elm_panel_hidden_set(leftPanel, (show ? EINA_FALSE : EINA_TRUE));
}

LIBAPI Eina_Bool TFC::Components::SidebarView::IsDrawerOpened()
{
	return !(elm_panel_hidden_get(leftPanel));
}

LIBAPI bool TFC::Components::SidebarView::BackButtonClicked()
{
	ToggleSidebar();
	return false;
}
