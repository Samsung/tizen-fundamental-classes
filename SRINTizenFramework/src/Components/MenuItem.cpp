/*
 * MenuItem.cpp
 *
 *  Created on: Apr 26, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/MenuItem.h"

using namespace TFC::Components;

MenuItem::MenuItem(std::string menuText, std::string menuIcon, void* itemData, CustomMenuStyle* customStyle) :
	parentMenu(nullptr), itemData(itemData), genlistItem(nullptr), expanded(false)
{
	Text = menuText;
	MenuIcon = menuIcon;
	CustomItemStyle = customStyle;
}

void MenuItem::AddSubMenu(MenuItem* subMenu)
{
	subMenu->SetParentMenu(this);
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

MenuItem* MenuItem::GetParentMenu() const {
	return parentMenu;
}

void MenuItem::SetParentMenu(MenuItem* parentMenu) {
	this->parentMenu = parentMenu;
}
