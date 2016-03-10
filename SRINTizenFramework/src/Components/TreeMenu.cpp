/*
 * TreeMenu.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Components/TreeMenu.h"

using namespace SRIN::Components;

typedef struct
{
	MenuItem* menuItemRef;
	TreeMenu* treeMenuRef;
} TreeMenuItemPackage;

MenuItem::MenuItem(std::string menuText, std::string menuIcon, void* itemData) :
	itemData(itemData), menuIcon(menuIcon), genlistItem(nullptr), expanded(false)
{
	MenuText = menuText;
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

void TreeMenu::MenuSelectedInternal(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* genlistItem)
{
	auto item = reinterpret_cast<MenuItem*>(elm_object_item_data_get(genlistItem));
	elm_genlist_item_selected_set(genlistItem, EINA_FALSE);

	auto expanded = elm_genlist_item_expanded_get(genlistItem);
	elm_genlist_item_expanded_set(genlistItem, !expanded);

	OnMenuSelected(this, item);
}

void TreeMenu::MenuExpanded(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* genlistItem)
{
	auto item = reinterpret_cast<MenuItem*>(elm_object_item_data_get(genlistItem));
	GenerateSubMenu(item);
	item->expanded = true;
	elm_genlist_item_fields_update(genlistItem, "elm.swallow.end", ELM_GENLIST_ITEM_FIELD_CONTENT);
}

void TreeMenu::MenuContracted(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* genlistItem)
{
	auto item = reinterpret_cast<MenuItem*>(elm_object_item_data_get(genlistItem));
	elm_genlist_item_subitems_clear(genlistItem);
	item->expanded = false;
	elm_genlist_item_fields_update(genlistItem, "elm.swallow.end", ELM_GENLIST_ITEM_FIELD_CONTENT);
}

Evas_Object* TreeMenu::CreateComponent(Evas_Object* root)
{
	genlist = elm_genlist_add(root);

	itemClass = elm_genlist_item_class_new();

	itemClass->item_style = "group_index/expandable";
	itemClass->func.text_get = [] (void *data, Evas_Object *obj, const char *part) -> char*
	{
		auto menuItem = reinterpret_cast<MenuItem*>(data);

		if (!strcmp("elm.text", part))
		{
			auto text = menuItem->MenuText->c_str();
			return strdup(text);
		}
		return nullptr;
	};

	submenuItemClass = elm_genlist_item_class_new();
	submenuItemClass->item_style = "type1";
	submenuItemClass->func.text_get = [] (void *data, Evas_Object *obj, const char *part) -> char*
	{
		auto menuItem = reinterpret_cast<MenuItem*>(data);

		if (!strcmp("elm.text", part))
		{
			auto text = menuItem->MenuText->c_str();
			return strdup(text);
		}
		return nullptr;
	};

	elm_genlist_block_count_set(genlist, 14);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);

	//evas_object_event_callback_add(genlist, EVAS_CALLBACK_MOUSE_DOWN, ObjectEventHandler, NULL);
	//evas_object_smart_callback_add(genlist, "realized", gl_realized_cb, NULL);
	//evas_object_smart_callback_add(genlist, "loaded", gl_loaded_cb, NULL);
	//evas_object_smart_callback_add(genlist, "longpressed", gl_longpressed_cb, NULL);
	evas_object_smart_callback_add(genlist, "selected", SmartEventHandler, &OnMenuSelectedInternal);
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

		item->genlistItem = genlistItem;
	}
}

TreeMenu::TreeMenu() :
	genlist(nullptr), itemClass(nullptr)

{
	OnMenuSelectedInternal +=  { this, &TreeMenu::MenuSelectedInternal };
	OnMenuExpanded += { this, &TreeMenu::MenuExpanded };
	OnMenuContracted += { this, &TreeMenu::MenuContracted };
}

void TreeMenu::AddMenu(MenuItem* menu)
{
	rootMenu.push_back(menu);

	if (genlist)
	{
		auto genlistItem = elm_genlist_item_append(genlist, // genlist object
			itemClass, // item class
			menu, // item class user data
			nullptr, // parent
			ELM_GENLIST_ITEM_TREE, // type
			nullptr, // callback
			menu);

		menu->genlistItem = genlistItem;
	}
}

TreeMenu::~TreeMenu()
{
	if (itemClass)
		elm_genlist_item_class_free(itemClass);
	itemClass = nullptr;
}

void TreeMenu::GenerateSubMenu(MenuItem* rootMenu)
{
	for (auto item : rootMenu->subMenus)
	{
		auto genlistItem = elm_genlist_item_append(genlist, // genlist object
			submenuItemClass, // item class
			item, // item class user data
			rootMenu->genlistItem, // parent
			ELM_GENLIST_ITEM_NONE, // type
			nullptr, // callback
			item);

		item->genlistItem = genlistItem;
	}
}

void SRIN::Components::TreeMenu::AddMenu(const std::vector<MenuItem*>& listOfMenus)
{
	for (auto item : listOfMenus)
	{
		AddMenu(item);
	}

}
