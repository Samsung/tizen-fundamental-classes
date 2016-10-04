/*
 * TreeMenu.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Components/TreeMenu.h"
#include "SRIN/Components/MenuItem.h"

#include <algorithm>

using namespace SRIN::Components;

struct TreeMenuItemPackage
{
	MenuItem* menuItemRef;
	TreeMenu* treeMenuRef;
	CustomMenuStyle* customMenuStyleRef;

	TreeMenuItemPackage(MenuItem* menuItem, TreeMenu* treeMenu, CustomMenuStyle* customMenuStyle = nullptr) :
		menuItemRef(menuItem), treeMenuRef(treeMenu), customMenuStyleRef(customMenuStyle)
	{

	}
};

void TreeMenu::MenuScrollInternal(GenlistEvent* eventSource,
		Evas_Object* objSource, Elm_Object_Item* genlistItem) {
	isScrolled = true;
}

void TreeMenu::MenuPressedInternal(GenlistEvent* eventSource,
		Evas_Object* objSource, Elm_Object_Item* genlistItem) {
	if (!isClickPersist) {
		auto icon = elm_object_item_part_content_get(genlistItem, "menu_icon");
		edje_object_signal_emit(icon, "elm,state,selected", "elm");
	}
}

void TreeMenu::MenuReleasedInternal(GenlistEvent* eventSource,
		Evas_Object* objSource, Elm_Object_Item* genlistItem) {
	if (!isClickPersist) {
		auto item = reinterpret_cast<TreeMenuItemPackage*>(elm_object_item_data_get(genlistItem));

		auto icon = elm_object_item_part_content_get(genlistItem, "menu_icon");
		edje_object_signal_emit(icon, "elm,state,unselected", "elm");

		if (!isScrolled) {
			OnMenuSelected(this, item->menuItemRef);
		} else {
			isScrolled = false;
		}
	}
}

void TreeMenu::MenuSelectedInternal(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* genlistItem)
{
	if (isClickPersist) {
		auto item = reinterpret_cast<TreeMenuItemPackage*>(elm_object_item_data_get(genlistItem));

		auto prevIcon = elm_object_item_part_content_get(currentlySelected, "menu_icon");
		edje_object_signal_emit(prevIcon, "elm,state,unselected", "elm");

		auto icon = elm_object_item_part_content_get(genlistItem, "menu_icon");
		edje_object_signal_emit(icon, "elm,state,selected", "elm");

		if(currentlySelected == genlistItem)
			return;

		currentlySelected = genlistItem;
		OnMenuSelected(this, item->menuItemRef);
	}
}

void TreeMenu::MenuUnselectedInternal(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* genlistItem)
{

}

void TreeMenu::MenuExpanded(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* genlistItem)
{
	auto item = reinterpret_cast<TreeMenuItemPackage*>(elm_object_item_data_get(genlistItem));
	GenerateSubMenu(item->menuItemRef);
	item->menuItemRef->expanded = true;
	elm_genlist_item_fields_update(genlistItem, "elm.swallow.end", ELM_GENLIST_ITEM_FIELD_CONTENT);
}

void TreeMenu::MenuContracted(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* genlistItem)
{
	auto item = reinterpret_cast<TreeMenuItemPackage*>(elm_object_item_data_get(genlistItem));
	elm_genlist_item_subitems_clear(genlistItem);
	item->menuItemRef->expanded = false;
	elm_genlist_item_fields_update(genlistItem, "elm.swallow.end", ELM_GENLIST_ITEM_FIELD_CONTENT);
}

Evas_Object* TreeMenu::CreateComponent(Evas_Object* root)
{
	genlist = elm_genlist_add(root);

	elm_genlist_select_mode_set(genlist, ELM_OBJECT_SELECT_MODE_ALWAYS);
	itemClass = elm_genlist_item_class_new();

	itemClass->item_style = "group_index/expandable";
	itemClass->func.text_get = [] (void *data, Evas_Object *obj, const char *part) -> char*
	{
		auto item = reinterpret_cast<TreeMenuItemPackage*>(data);

		if (!strcmp("elm.text", part))
		{
			auto text = item->menuItemRef->Text->c_str();
			return strdup(text);
		}
		return nullptr;
	};
	itemClass->func.del = [] (void* data, Evas_Object* evas)
	{
		auto item = reinterpret_cast<TreeMenuItemPackage*>(data);
		delete item;
	};
	itemClass->func.content_get = [] (void *data, Evas_Object *obj, const char *part) -> Evas_Object*
	{
		auto item = reinterpret_cast<TreeMenuItemPackage*>(data);
		auto menuItem = item->menuItemRef;
		auto thisTreeMenu = item->treeMenuRef;

		dlog_print(DLOG_DEBUG, LOG_TAG, "Reaching %s part", part);
		if (menuItem->GetSubMenus().size() && !strcmp("button_expand", part))
		{
			auto button = elm_button_add(obj);
			evas_object_smart_callback_add(button, "clicked", [] (void* data, Evas_Object* obj, void* eventData) {
				auto item = reinterpret_cast<TreeMenuItemPackage*>(data);
				auto genlistItem = item->menuItemRef->genlistItem;
				elm_genlist_item_selected_set(genlistItem, EINA_FALSE);
				auto expanded = elm_genlist_item_expanded_get(genlistItem);
				elm_genlist_item_expanded_set(genlistItem, !expanded);

				if(expanded)
					elm_object_signal_emit(obj, "expandButton", "srin");
				else
					elm_object_signal_emit(obj, "collapseButton", "srin");

				// Refresh the selected state on currently selected object item
				// Bug in EFL which it emits default signal inapproriately as this button is clicked, which
				// results in deselected state of the root menu

				elm_genlist_item_selected_set(item->treeMenuRef->currentlySelected, true);

			}, data);

			// Refresh state when recreating
			elm_object_style_set(button, "circle");

			auto expanded = elm_genlist_item_expanded_get(menuItem->genlistItem);
			if(!expanded)
				elm_object_signal_emit(button, "expandButton", "srin");
			else
				elm_object_signal_emit(button, "collapseButton", "srin");

			evas_object_event_callback_add(button, EVAS_CALLBACK_MOUSE_DOWN,
				[] (void* data, Evas* evas, Evas_Object* obj, void* eventData) { }, nullptr);
			evas_object_propagate_events_set(button, EINA_FALSE);
			evas_object_repeat_events_set(button, EINA_FALSE);


			return button;
		}
		else if(!strcmp("menu_icon", part))
		{
			if(thisTreeMenu->IconEdjeFile->length() != 0)
			{

				if(menuItem->MenuIcon->length() != 0)
				{
					auto icon = edje_object_add(obj);
					edje_object_file_set(icon, thisTreeMenu->IconEdjeFile->c_str(), menuItem->MenuIcon->c_str());

					if(item->menuItemRef->genlistItem == item->treeMenuRef->currentlySelected)
						edje_object_signal_emit(icon, "elm,state,selected", "elm");

					return icon;
				}
			}
		}
		return nullptr;
	};

	submenuItemClass = elm_genlist_item_class_new();
	submenuItemClass->item_style = "type1";
	submenuItemClass->func.text_get = [] (void *data, Evas_Object *obj, const char *part) -> char*
	{
		auto item = reinterpret_cast<TreeMenuItemPackage*>(data);

		if (!strcmp("elm.text", part))
		{
			auto text = item->menuItemRef->Text->c_str();
			return strdup(text);
		}
		return nullptr;
	};
	submenuItemClass->func.content_get = [] (void *data, Evas_Object *obj, const char *part) -> Evas_Object*
	{
		auto item = reinterpret_cast<TreeMenuItemPackage*>(data);
		auto menuItem = item->menuItemRef;
		auto thisTreeMenu = item->treeMenuRef;

		dlog_print(DLOG_DEBUG, LOG_TAG, "Reaching %s part", part);

		if(!strcmp("menu_icon", part))
		{
			if(thisTreeMenu->IconEdjeFile->length() != 0)
			{

				if(menuItem->MenuIcon->length() != 0)
				{
					auto icon = edje_object_add(obj);
					edje_object_file_set(icon, thisTreeMenu->IconEdjeFile->c_str(), menuItem->MenuIcon->c_str());

					return icon;
				}
			}
		}
		return nullptr;
	};
	itemClass->func.del = [] (void* data, Evas_Object* evas)
	{
		auto item = reinterpret_cast<TreeMenuItemPackage*>(data);
		delete item;
	};

	elm_genlist_block_count_set(genlist, 14);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);

	evas_object_smart_callback_add(genlist, "scroll", EFL::EvasSmartEventHandler, &eventMenuScrollInternal);

	evas_object_smart_callback_add(genlist, "pressed", EFL::EvasSmartEventHandler, &eventMenuPressedInternal);
	evas_object_smart_callback_add(genlist, "released", EFL::EvasSmartEventHandler, &eventMenuReleasedInternal);

	evas_object_smart_callback_add(genlist, "selected", EFL::EvasSmartEventHandler, &eventMenuSelectedInternal);
	evas_object_smart_callback_add(genlist, "unselected", EFL::EvasSmartEventHandler, &eventMenuUnselectedInternal);

	evas_object_smart_callback_add(genlist, "expanded", EFL::EvasSmartEventHandler, &eventMenuExpanded);
	evas_object_smart_callback_add(genlist, "contracted", EFL::EvasSmartEventHandler, &eventMenuContracted);

	GenerateRootMenu();

	return genlist;
}

void TreeMenu::GenerateRootMenu()
{
	for (auto menu : rootMenu)
	{
		AppendMenuToGenlist(menu);
	}
}

TreeMenu::TreeMenu() :
	genlist(nullptr), itemClass(nullptr), submenuItemClass(nullptr),
	currentlySelected(nullptr), autoExpand(false),
	MenuItems(this), AutoExpanded(this)

{
	eventMenuScrollInternal +=  EventHandler(TreeMenu::MenuScrollInternal);

	eventMenuPressedInternal +=  EventHandler(TreeMenu::MenuPressedInternal);
	eventMenuReleasedInternal += EventHandler(TreeMenu::MenuReleasedInternal);

	eventMenuSelectedInternal +=  EventHandler(TreeMenu::MenuSelectedInternal);
	eventMenuUnselectedInternal += EventHandler(TreeMenu::MenuUnselectedInternal);

	eventMenuExpanded += EventHandler(TreeMenu::MenuExpanded);
	eventMenuContracted += EventHandler(TreeMenu::MenuContracted);

	isClickPersist = true;
	isScrolled = false;
}

void TreeMenu::AppendMenuToGenlist(MenuItem* menu)
{
	Elm_Object_Item* genlistItem = nullptr;
	CustomMenuStyle* customStyle = menu->CustomItemStyle;
	if (!customStyle)
	{
		genlistItem = elm_genlist_item_append(genlist, // genlist object
			itemClass, // item class
			new TreeMenuItemPackage(
			{ menu, this }), // item class user data
			nullptr, // parent
			ELM_GENLIST_ITEM_TREE, // type
			nullptr, // callback
			menu);
	}
	else
	{
		genlistItem = elm_genlist_item_append(genlist, // genlist object
			*customStyle, // item class
			new TreeMenuItemPackage(
			{ menu, this, customStyle }), // item class user data
			nullptr, // parent
			ELM_GENLIST_ITEM_TREE, // type
			nullptr, // callback
			menu);
	}
	if (autoExpand)
		elm_genlist_item_expanded_set(genlistItem, EINA_TRUE);

	menu->genlistItem = genlistItem;
}

void TreeMenu::AddMenu(MenuItem* menu)
{
	rootMenu.push_back(menu);

	if (genlist)
	{
		AppendMenuToGenlist(menu);
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
		Elm_Object_Item* genlistItem = nullptr;
		CustomMenuStyle* customStyle = item->CustomItemStyle;
		if (!customStyle)
		{
			genlistItem = elm_genlist_item_append(genlist, // genlist object
				submenuItemClass, // item class
				new TreeMenuItemPackage({ item, this }), // item class user data
				rootMenu->genlistItem, // parent
				ELM_GENLIST_ITEM_NONE, // type
				nullptr, // callback
				item);
		} else {
			genlistItem = elm_genlist_item_append(genlist, // genlist object
				*customStyle, // item class
				new TreeMenuItemPackage({ item, this, customStyle }), // item class user data
				rootMenu->genlistItem, // parent
				ELM_GENLIST_ITEM_NONE, // type
				nullptr, // callback
				item);
		}

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

SRIN::Components::CustomMenuStyle::CustomMenuStyle(CString style)
{
	this->customStyle = elm_genlist_item_class_new();
	this->customStyle->item_style = style;
	this->customStyle->func.content_get = [] (void *data, Evas_Object *obj, const char *part) -> Evas_Object*
	{
		auto pkg = reinterpret_cast<TreeMenuItemPackage*>(data);
		return pkg->customMenuStyleRef->GetContent(pkg->menuItemRef, obj, part);
	};

	this->customStyle->func.text_get = [] (void *data, Evas_Object *obj, const char *part) -> char*
	{
		auto pkg = reinterpret_cast<TreeMenuItemPackage*>(data);
		return strdup(pkg->customMenuStyleRef->GetString(pkg->menuItemRef, obj, part).c_str());
	};

	// Callback redirect for delete function
	this->customStyle->func.del = [] (void *data, Evas_Object *obj)
	{
		auto pkg = reinterpret_cast<TreeMenuItemPackage*>(data);
		delete pkg;
	};
}

SRIN::Components::CustomMenuStyle::operator Elm_Genlist_Item_Class*()
{
	return this->customStyle;
}

SRIN::Components::CustomMenuStyle::~CustomMenuStyle()
{
	elm_genlist_item_class_free(this->customStyle);
}

const std::vector<MenuItem*>& SRIN::Components::TreeMenu::GetMenuItems()
{
	return this->rootMenu;
}

void SRIN::Components::TreeMenu::AddMenuAt(int index, MenuItem* menu)
{
	if(index >= this->rootMenu.size())
		AddMenu(menu);
	else
	{
		auto pos = this->rootMenu.begin() + index;

		if(this->genlist)
		{
			elm_genlist_item_insert_after(
				this->genlist,
				Coalesce<Elm_Genlist_Item_Class>(*menu->CustomItemStyle, this->itemClass),
				IsNull<CustomMenuStyle>(menu->CustomItemStyle) ?
					new TreeMenuItemPackage({ menu, this }) :
					new TreeMenuItemPackage({ menu, this, menu->CustomItemStyle }) ,
				nullptr, // parent
				(*pos)->genlistItem,
				ELM_GENLIST_ITEM_TREE, // type
				nullptr, // callback
				menu);
		}

		this->rootMenu.insert(pos, menu);
	}
}

bool SRIN::Components::TreeMenu::GetAutoExpanded()
{
	return autoExpand;
}

void SRIN::Components::TreeMenu::SetAutoExpanded(const bool& val)
{
	autoExpand = val;
}

void SRIN::Components::TreeMenu::RemoveMenu(MenuItem* menu)
{
	auto pos = std::find(rootMenu.begin(), rootMenu.end(), menu);
	rootMenu.erase(pos);

}

void SRIN::Components::TreeMenu::ResetCurrentlySelectedItem()
{
	currentlySelected = nullptr;
}
