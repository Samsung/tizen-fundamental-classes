/*
 * ContextMenu.h
 *
 *  Created on: Apr 26, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_COMPONENTS_CONTEXTMENU_H_
#define SRIN_COMPONENTS_CONTEXTMENU_H_

#include "SRIN/Components/BackButtonHandler.h"
#include "SRIN/Components/ComponentBase.h"
#include "SRIN/Components/MenuItem.h"


#include <vector>

namespace SRIN {
namespace Components {

/**
 * Component that provides context menu on the naviframe.
 */
class LIBAPI ContextMenu: public ComponentBase, BackButtonHandler
{
public:
	struct ContextMenuPackage
	{
		ContextMenu* thisRef;
		MenuItem* menuItemRef;
		void RaiseEvent();
	};

private:
	friend struct ContextMenuPackage;

	std::vector<ContextMenuPackage*> currentItemPackages;

	std::string text;
	Evas_Object* naviframe;
	Evas_Object* button;
	Evas_Object* contextMenu;

	EFL::EvasSmartEvent eventContextMenuButtonClicked;
	EFL::EvasSmartEvent eventContextMenuDismissed;

	bool menuShown;
	std::vector<MenuItem*> rootMenu;

	void OnContextMenuButtonClicked(EFL::EvasSmartEvent* ev, Evas_Object* obj, void* eventData);
	void OnContextMenuDismissed(EFL::EvasSmartEvent* ev, Evas_Object* obj, void* eventData);
	bool BackButtonClicked();
	void ShowMenu();
	void HideMenu();

	void RaiseOnClickEvent(MenuItem* menuItemRef);

protected:
	/**
	 * Method overriden from ComponentBase, creates the UI elements of the component.
	 *
	 * @param root The root/parent given for this component.
	 *
	 * @return The context menu button on the naviframe.
	 */
	virtual Evas_Object* CreateComponent(Evas_Object* root);
	void SetText(const std::string& text);
	std::string& GetText();

public:
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

	Property<ContextMenu, std::string&>::GetSet<&ContextMenu::GetText, &ContextMenu::SetText> Text;
};

}
}



#endif /* CONTEXTMENU_H_ */
