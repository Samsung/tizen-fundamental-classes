/*
 * TreeMenu.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: gilang
 */

#include "SRIN/Components/TreeMenu.h"

using namespace SRIN::Components;

typedef struct
{
	MenuItem* menuItemRef;
	TreeMenu* treeMenuRef;
} TreeMenuItemPackage;

MenuItem::MenuItem(std::string menuText, std::string menuIcon) :
	itemData(nullptr), OnMenuItemClick(this), menuText(menuText), menuIcon(menuIcon)
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

	itemClass = elm_genlist_item_class_new();

	itemClass->item_style = "group_index/expandable";
	itemClass->func.text_get = [] (void *data, Evas_Object *obj, const char *part)
	{
		auto menuItem = reinterpret_cast<MenuItem*>(data);

		if (!strcmp("elm.text", part))
		{
			auto text = menuItem->menuText.c_str();
			return strdup(text);
		}
	};

	elm_genlist_block_count_set(genlist, 14);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);

	//evas_object_event_callback_add(genlist, EVAS_CALLBACK_MOUSE_DOWN, ObjectEventHandler, NULL);
	//evas_object_smart_callback_add(genlist, "realized", gl_realized_cb, NULL);
	//evas_object_smart_callback_add(genlist, "loaded", gl_loaded_cb, NULL);
	//evas_object_smart_callback_add(genlist, "longpressed", gl_longpressed_cb, NULL);
	evas_object_smart_callback_add(genlist, "selected", SmartEventHandler, &OnMenuSelected);
	evas_object_smart_callback_add(genlist, "expanded", SmartEventHandler, &OnMenuExpanded);
	evas_object_smart_callback_add(genlist, "contracted", SmartEventHandler, &OnMenuContracted);

	GenerateRootMenu();

	return genlist;
}

void TreeMenu::GenerateRootMenu()
{
	for (auto item : rootMenu)
	{
		auto genlistItem = elm_genlist_item_append(genlist, // genlist object
			itemClass, // item class
			item, // item class user data
			nullptr, // parent
			ELM_GENLIST_ITEM_TREE, // type
			nullptr, // callback
			item);
	}
}

TreeMenu::TreeMenu() :
	genlist(nullptr), itemClass(nullptr), OnMenuSelected(this, (EventHandler) &TreeMenu::MenuSelected), OnMenuExpanded(
		this, (EventHandler) &TreeMenu::MenuExpanded), OnMenuContracted(this, (EventHandler) &TreeMenu::MenuContracted)

{

}

void SRIN::Components::TreeMenu::AddMenu(MenuItem* menu)
{
	rootMenu.push_back(menu);
}

SRIN::Components::TreeMenu::~TreeMenu()
{
	if (itemClass)
		elm_genlist_item_class_free(itemClass);
	itemClass = nullptr;
}
