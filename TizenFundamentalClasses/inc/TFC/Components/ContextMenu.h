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
 *    Components/ContextMenu.h
 *
 * Components to create context menuW
 *
 * Created on: 	 Apr 28, 2016
 * Author: 		 Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributors: Ida Bagus Putu Peradnya Dinata (ib.putu@samsung.com)
 * 				 Kevin Winata (k.winata@samsung.com)
 */

#ifndef TFC_COMPONENTS_CONTEXTMENU_H_
#define TFC_COMPONENTS_CONTEXTMENU_H_

#include "TFC/Components/BackButtonHandler.h"
#include "TFC/Components/ComponentBase.h"
#include "TFC/Components/MenuItem.h"


#include <vector>

namespace TFC {
namespace Components {

/**
 * Component that provides context menu on the naviframe.
 */
class LIBAPI ContextMenu:
		public BackButtonHandler,
		public ComponentBase,
		public EFL::EFLProxyClass,
		PropertyClass<ContextMenu>
{
	using PropertyClass<ContextMenu>::Property;
public:
	struct ContextMenuPackage
	{
		ContextMenu* thisRef;
		MenuItem* menuItemRef;
		void RaiseEvent();
	};

	/**
	 * Constructor of ContextMenu.
	 */
	ContextMenu();

	/**
	 * Method to add MenuItem to the context menu.
	 *
	 * @param menu MenuItem that will be added.
	 */
	void AddMenu(MenuItem* menu);

	/**
	 * Method to add MenuItem at a specific position to the context menu.
	 *
	 * @param index Index that indicates where the item will be placed.
	 * @param menu MenuItem that will be added.
	 */
	void AddMenuAt(int index, MenuItem* menu);

	/**
	 * Method to remove a particular MenuItem from the context menu.
	 *
	 * @param menu MenuItem that will be removed.
	 */
	void RemoveMenu(MenuItem* menu);

	/**
	 * Method to bulk-add MenuItems to the context menu.
	 *
	 * @param listOfMenus Vector that contains MenuItems that will be added.
	 */
	void AddMenu(const std::vector<MenuItem*>& listOfMenus);

	/**
	 * Method to set list of MenuItems as menus.
	 *
	 * @param listOfMenus Vector that contains MenuItems that will be set.
	 */
	void SetMenu(const std::vector<MenuItem*>& listOfMenus);

	/**
	 * Destructor of ContextMenu.
	 */
	virtual ~ContextMenu();

protected:
	/**
	 * Method overriden from ComponentBase, creates the UI elements of the component.
	 *
	 * @param root The root/parent given for this component.
	 *
	 * @return The context menu button on the naviframe.
	 */
	virtual Evas_Object* CreateComponent(Evas_Object* root) override;
	void SetText(std::string const& text);
	std::string const& GetText() const;

private:
	friend struct ContextMenuPackage;
	std::string text;

	std::vector<ContextMenuPackage*> currentItemPackages;
	std::vector<MenuItem*> rootMenu;

	EvasSmartEvent eventContextMenuButtonClicked;
	EvasSmartEvent eventContextMenuDismissed;

	Evas_Object* naviframe;
	Evas_Object* button;
	Evas_Object* contextMenu;

	bool menuShown;

	bool BackButtonClicked() override;

	void OnContextMenuButtonClicked(Evas_Object* obj, void* eventData);
	void OnContextMenuDismissed(Evas_Object* obj, void* eventData);
	void ShowMenu();
	void HideMenu();

	void RaiseOnClickEvent(MenuItem* menuItemRef);

public:
	Property<std::string const&>::Get<&ContextMenu::GetText>::Set<&ContextMenu::SetText> Text;
};

}}



#endif /* CONTEXTMENU_H_ */
