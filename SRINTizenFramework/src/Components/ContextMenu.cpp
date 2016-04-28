/*
 * ContextMenu.cpp
 *
 *  Created on: Apr 26, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Components/ContextMenu.h"

#include <algorithm>

Evas_Object* SRIN::Components::ContextMenu::CreateComponent(Evas_Object* root)
{
	naviframe = root;
	button = elm_button_add(root);
	evas_object_smart_callback_add(button, "clicked", SmartEventHandler, &eventContextMenuButtonClicked);
	return button;
}

void SRIN::Components::ContextMenu::SetText(const std::string& text)
{
}

std::string& SRIN::Components::ContextMenu::GetText()
{
}

SRIN::Components::ContextMenu::ContextMenu() :  naviframe(nullptr), button(nullptr), menuShown(false), Text(this)
{
	eventContextMenuButtonClicked += AddEventHandler(ContextMenu::OnContextMenuButtonClicked);
}

void SRIN::Components::ContextMenu::AddMenu(MenuItem* menu)
{
	rootMenu.push_back(menu);
}

void SRIN::Components::ContextMenu::AddMenuAt(int index, MenuItem* menu)
{
	if(index >= this->rootMenu.size())
		AddMenu(menu);
	else
	{
		auto pos = this->rootMenu.begin() + index;
		this->rootMenu.insert(pos, menu);
	}
}

void SRIN::Components::ContextMenu::RemoveMenu(MenuItem* menu)
{
	auto pos = std::find(rootMenu.begin(), rootMenu.end(), menu);
	rootMenu.erase(pos);
}

void SRIN::Components::ContextMenu::AddMenu(const std::vector<MenuItem*>& listOfMenus)
{
	for (auto item : listOfMenus)
	{
		this->AddMenu(item);
	}
}

void SRIN::Components::ContextMenu::OnContextMenuButtonClicked(ElementaryEvent* ev, Evas_Object* obj, void* eventData)
{
	if(not this->menuShown)
		this->ShowMenu();
	else
		this->HideMenu();
}

void SRIN::Components::ContextMenu::ShowMenu()
{
	auto contextMenu = elm_ctxpopup_add(this->naviframe);

	BackButtonHandler::Acquire();

	this->contextMenu = contextMenu;
}

void SRIN::Components::ContextMenu::HideMenu()
{
	if(this->contextMenu != nullptr)
	{
		evas_object_del(this->contextMenu);
		this->contextMenu = nullptr;
	}
}


bool SRIN::Components::ContextMenu::BackButtonClicked()
{
	HideMenu();
	return false;
}

SRIN::Components::ContextMenu::~ContextMenu()
{
}
