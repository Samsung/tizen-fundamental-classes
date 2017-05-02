/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Components/MenuItem.cpp
 *
 * Created on:  Apr 26, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/MenuItem.h"

using namespace TFC::Components;

MenuItem::MenuItem(std::string menuText, std::string menuIcon, void* itemData, CustomMenuStyle* customStyle) :
	menuText(menuText),
	menuIcon(menuIcon),
	parentMenu(nullptr),
	itemData(itemData),
	genlistItem(nullptr),
	customStyle(customStyle),
	expanded(false),
	Text(this),
	MenuIcon(this),
	CustomItemStyle(this)
{

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
