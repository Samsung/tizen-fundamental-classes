/*
 * FloatingMenu.cpp
 *
 *  Created on: May 20, 2016
 *      Author: Kevin Winata
 */

#include "SRIN/Components/FloatingMenu.h"
#include <efl_extension.h>
#include <algorithm>

SRIN::Components::FloatingMenu::FloatingMenu() :
	naviframe(nullptr),
	floatingLayout(nullptr),
	button(nullptr),
	contextMenu(nullptr),
	menuShown(false),
	buttonImage("")
{
	eventButtonClicked += AddEventHandler(FloatingMenu::OnButtonClicked);
	eventContextMenuDismissed += AddEventHandler(FloatingMenu::OnContextMenuDismissed);
}

Evas_Object* SRIN::Components::FloatingMenu::CreateComponent(Evas_Object* root)
{
	naviframe = root;

	floatingLayout = eext_floatingbutton_add(naviframe);
	elm_object_part_content_set(naviframe, "elm.swallow.floatingbutton", floatingLayout);

	button = elm_button_add(floatingLayout);
	elm_object_part_content_set(floatingLayout, "button1", button);
	evas_object_smart_callback_add(button, "clicked", &EFL::EvasSmartEventHandler, &eventButtonClicked);

	if (buttonImage.size() > 0) {
		auto image = elm_image_add(floatingLayout);
		std::string path = SRIN::Framework::ApplicationBase::GetResourcePath(buttonImage.c_str());
		elm_image_file_set(image, path.c_str(), NULL);
		elm_object_part_content_set(button, "icon", image);
	}

	return floatingLayout;
}

void FloatingMenu_ItemClickHandler(void* data, Evas_Object* obj, void* eventData)
{
	auto pkg = reinterpret_cast<SRIN::Components::FloatingMenu::ContextMenuPackage*>(data);
	pkg->RaiseEvent();
}

void SRIN::Components::FloatingMenu::AddMenu(MenuItem* menu)
{
	rootMenu.push_back(menu);
}

void SRIN::Components::FloatingMenu::AddMenuAt(int index, MenuItem* menu)
{
	if(index >= this->rootMenu.size())
		AddMenu(menu);
	else
	{
		auto pos = this->rootMenu.begin() + index;
		this->rootMenu.insert(pos, menu);
	}
}

void SRIN::Components::FloatingMenu::RemoveMenu(MenuItem* menu)
{
	auto pos = std::find(rootMenu.begin(), rootMenu.end(), menu);
	rootMenu.erase(pos);
}

void SRIN::Components::FloatingMenu::AddMenu(const std::vector<MenuItem*>& listOfMenus)
{
	for (auto item : listOfMenus)
	{
		this->AddMenu(item);
	}
}

void SRIN::Components::FloatingMenu::SetMenu(const std::vector<MenuItem*>& listOfMenus)
{
	rootMenu = listOfMenus;
}

bool SRIN::Components::FloatingMenu::BackButtonClicked()
{
	elm_ctxpopup_dismiss(this->contextMenu);
	return false;
}

void SRIN::Components::FloatingMenu::OnButtonClicked(EFL::EvasSmartEvent* ev, Evas_Object* obj, void* eventData)
{
	if(not this->menuShown)
		this->ShowMenu();
	else
		this->HideMenu();
}

void SRIN::Components::FloatingMenu::OnContextMenuDismissed(EFL::EvasSmartEvent* ev, Evas_Object* obj, void* eventData)
{
	HideMenu();
}

void SRIN::Components::FloatingMenu::ShowMenu()
{
	BackButtonHandler::Acquire();

	auto contextMenu = elm_ctxpopup_add(this->naviframe);
	evas_object_smart_callback_add(contextMenu, "dismissed", EFL::EvasSmartEventHandler, &eventContextMenuDismissed);
	elm_object_style_set(contextMenu, "dropdown/label");

	this->contextMenu = contextMenu;

	this->menuShown = true;
	for(auto item : rootMenu)
	{
		Evas_Object* img = nullptr;
		if(item->MenuIcon->length())
		{
			img = elm_image_add(contextMenu);
			elm_image_file_set(img, Framework::ApplicationBase::GetResourcePath(item->MenuIcon->c_str()).c_str(), nullptr);
		}
		auto itemPackage = new ContextMenuPackage({this, item});
		this->currentItemPackages.push_back(itemPackage);
		elm_ctxpopup_item_append(contextMenu, item->Text->c_str(), img, FloatingMenu_ItemClickHandler, itemPackage);
	}

	Evas_Coord x, y, w , h;
	evas_object_geometry_get(this->button, &x, &y, &w, &h);
	elm_ctxpopup_direction_priority_set(contextMenu,
			ELM_CTXPOPUP_DIRECTION_DOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN,
			ELM_CTXPOPUP_DIRECTION_UNKNOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN);

	evas_object_move(contextMenu, x, y + h);

	evas_object_show(contextMenu);
}

void SRIN::Components::FloatingMenu::HideMenu()
{
	if(this->contextMenu != nullptr)
	{
		evas_object_del(this->contextMenu);
		this->contextMenu = nullptr;

		for(auto pkg : this->currentItemPackages)
		{
			delete pkg;
		}

		this->currentItemPackages.clear();
	}
	this->menuShown = false;
	BackButtonHandler::Release();
}

SRIN::Components::FloatingMenu::~FloatingMenu()
{
	HideMenu();
}

void SRIN::Components::FloatingMenu::ContextMenuPackage::RaiseEvent() {
	this->thisRef->RaiseOnClickEvent(this->menuItemRef);
}

void SRIN::Components::FloatingMenu::RaiseOnClickEvent(MenuItem* menuItemRef) {
	elm_ctxpopup_dismiss(this->contextMenu);
	menuItemRef->OnMenuItemClick(menuItemRef, nullptr);
}



