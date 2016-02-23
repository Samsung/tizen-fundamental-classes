/*
 * TreeMenu.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: gilang
 */

#include "SRIN/Components/TreeMenu.h"

using namespace SRIN::Components;

MenuItem::MenuItem(std::string menuText, std::string menuIcon) :
	itemData(nullptr),
	OnMenuItemClick(this)
{
}

void MenuItem::AddSubMenu(MenuItem* subMenu)
{
	subMenus.push_back(subMenu);
}

void MenuItem::RemoveSubMenu(int index)
{
	subMenus.erase(subMenus.begin() + index);
}

void MenuItem::GenerateSubMenu()
{

}

const std::vector<MenuItem*>& MenuItem::GetSubMenus() const
{
	return subMenus;
}

void TreeMenu::MenuSelected(Event* eventSource, Evas_Object* objSource, void* eventData)
{
}

void TreeMenu::MenuExpanded(Event* eventSource, Evas_Object* objSource, void* eventData)
{
}

void TreeMenu::MenuContracted(Event* eventSource, Evas_Object* objSource, void* eventData)
{
}

Evas_Object* TreeMenu::CreateComponent(Evas_Object* root)
{
	genlist = elm_genlist_add(root);

	elm_genlist_block_count_set(genlist, 14);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);

	//evas_object_event_callback_add(genlist, EVAS_CALLBACK_MOUSE_DOWN, ObjectEventHandler, NULL);
	//evas_object_smart_callback_add(genlist, "realized", gl_realized_cb, NULL);
	//evas_object_smart_callback_add(genlist, "loaded", gl_loaded_cb, NULL);
	//evas_object_smart_callback_add(genlist, "longpressed", gl_longpressed_cb, NULL);
	evas_object_smart_callback_add(genlist, "selected",   SmartEventHandler, &OnMenuSelected);
	evas_object_smart_callback_add(genlist, "expanded",   SmartEventHandler, &OnMenuExpanded);
	evas_object_smart_callback_add(genlist, "contracted", SmartEventHandler, &OnMenuContracted);

	GenerateRootMenu();

	return genlist;
}

void TreeMenu::GenerateRootMenu()
{
	for(auto item : rootMenu)
	{
		auto genlistItem = elm_genlist_item_append(
								genlist, // genlist object
								nullptr, // item class
								item, // item class user data
								nullptr, // parent
								ELM_GENLIST_ITEM_TREE, // type
								nullptr, // callback
								item);


	}
}

TreeMenu::TreeMenu() :
	OnMenuContracted(this, (EventHandler)&TreeMenu::MenuContracted),
	OnMenuExpanded(this, (EventHandler)&TreeMenu::MenuExpanded),
	OnMenuSelected(this, (EventHandler)&TreeMenu::MenuSelected)
{

}
