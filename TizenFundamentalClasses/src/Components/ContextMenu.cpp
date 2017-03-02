/*
 * ContextMenu.cpp
 *
 *  Created on: Apr 26, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        ib.putu (ib.putu@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/ContextMenu.h"
#include <algorithm>

Evas_Object* TFC::Components::ContextMenu::CreateComponent(Evas_Object* root)
{
	naviframe = root;
	button = elm_button_add(root);
	//evas_object_smart_callback_add(button, "clicked", EFL::EvasSmartEventHandler, &eventContextMenuButtonClicked);
	eventContextMenuButtonClicked.Bind(button, "clicked");
	elm_object_style_set(button, "naviframe/more/default");
	return button;
}

void ContextMenu_ItemClickHandler(void* data, Evas_Object* obj, void* eventData)
{
	auto pkg = reinterpret_cast<TFC::Components::ContextMenu::ContextMenuPackage*>(data);
	pkg->RaiseEvent();
}

void TFC::Components::ContextMenu::SetText(std::string const& text)
{
	this->text = text;
}

std::string const& TFC::Components::ContextMenu::GetText() const
{
	return text;
}

TFC::Components::ContextMenu::ContextMenu() :
	naviframe(nullptr),
	button(nullptr),
	contextMenu(nullptr),
	menuShown(false),
	Text(this)
{
	eventContextMenuButtonClicked += EventHandler(ContextMenu::OnContextMenuButtonClicked);
	eventContextMenuDismissed += EventHandler(ContextMenu::OnContextMenuDismissed);
}

void TFC::Components::ContextMenu::AddMenu(MenuItem* menu)
{
	rootMenu.push_back(menu);
}

void TFC::Components::ContextMenu::AddMenuAt(int index, MenuItem* menu)
{
	if(index >= this->rootMenu.size())
		AddMenu(menu);
	else
	{
		auto pos = this->rootMenu.begin() + index;
		this->rootMenu.insert(pos, menu);
	}
}

void TFC::Components::ContextMenu::RemoveMenu(MenuItem* menu)
{
	auto pos = std::find(rootMenu.begin(), rootMenu.end(), menu);
	rootMenu.erase(pos);
}

void TFC::Components::ContextMenu::AddMenu(const std::vector<MenuItem*>& listOfMenus)
{
	for (auto item : listOfMenus)
	{
		this->AddMenu(item);
	}
}

void TFC::Components::ContextMenu::SetMenu(const std::vector<MenuItem*>& listOfMenus)
{
	rootMenu = listOfMenus;
}

void TFC::Components::ContextMenu::OnContextMenuButtonClicked(Evas_Object* obj, void* eventData)
{
	if(not this->menuShown)
		this->ShowMenu();
	else
		this->HideMenu();
}

void TFC::Components::ContextMenu::ShowMenu()
{
	BackButtonHandler::Acquire();

	auto contextMenu = elm_ctxpopup_add(this->naviframe);
	//evas_object_smart_callback_add(contextMenu, "dismissed", EFL::EvasSmartEventHandler, &eventContextMenuDismissed);
	eventContextMenuDismissed.Bind(contextMenu, "dismissed");
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
		elm_ctxpopup_item_append(contextMenu, item->Text->c_str(), img, ContextMenu_ItemClickHandler, itemPackage);
	}

	Evas_Coord x, y, w , h;
	evas_object_geometry_get(this->button, &x, &y, &w, &h);
	elm_ctxpopup_direction_priority_set(contextMenu,
			ELM_CTXPOPUP_DIRECTION_DOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN,
			ELM_CTXPOPUP_DIRECTION_UNKNOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN);


	evas_object_move(contextMenu, x, y + h);

	evas_object_show(contextMenu);
}

void TFC::Components::ContextMenu::HideMenu()
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

bool TFC::Components::ContextMenu::BackButtonClicked()
{
	elm_ctxpopup_dismiss(this->contextMenu);
	return false;
}

TFC::Components::ContextMenu::~ContextMenu()
{
	HideMenu();
}

void TFC::Components::ContextMenu::OnContextMenuDismissed(Evas_Object* obj, void* eventData)
{
	HideMenu();
}

void TFC::Components::ContextMenu::ContextMenuPackage::RaiseEvent() {
	this->thisRef->RaiseOnClickEvent(this->menuItemRef);
}

void TFC::Components::ContextMenu::RaiseOnClickEvent(MenuItem* menuItemRef) {
	elm_ctxpopup_dismiss(this->contextMenu);
	menuItemRef->eventMenuItemClick(menuItemRef, nullptr);
}
