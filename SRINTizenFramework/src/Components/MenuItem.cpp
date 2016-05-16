/*
 * MenuItem.cpp
 *
 *  Created on: Apr 26, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Components/MenuItem.h"

using namespace SRIN::Components;

MenuItem::MenuItem(std::string menuText, std::string menuIcon, void* itemData, CustomMenuStyle* customStyle) :
	itemData(itemData), genlistItem(nullptr), expanded(false)
{
	Text = menuText;
	MenuIcon = menuIcon;
	CustomItemStyle = customStyle;
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

Elm_Object_Item* MenuItem::GetGenlistItem()
{
	return genlistItem;
}
