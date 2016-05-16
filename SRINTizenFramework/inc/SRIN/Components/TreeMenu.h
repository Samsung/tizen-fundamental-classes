/*
 * TreeMenu.h
 *
 *  Created on: Feb 23, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRINFW_TREEMENU_H_
#define SRINFW_TREEMENU_H_

#include <vector>
#include <string>

#include "SRIN/Components/ComponentBase.h"
#include "SRIN/Components/MenuItem.h"

namespace SRIN {
namespace Components {

class LIBAPI TreeMenu: virtual public ComponentBase
{
private:
	Evas_Object* genlist;
	Elm_Genlist_Item_Class* itemClass;
	Elm_Genlist_Item_Class* submenuItemClass;
	Elm_Object_Item* currentlySelected;
	std::vector<MenuItem*> rootMenu;

	bool isScrolled;
	bool autoExpand;

	void GenerateRootMenu();
	void GenerateSubMenu(MenuItem* subMenu);
	const std::vector<MenuItem*>& GetMenuItems();

	bool GetAutoExpanded();
	void SetAutoExpanded(const bool& val);

	typedef Event<Evas_Object*, Elm_Object_Item*> GenlistEvent;

	void MenuScrollInternal(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
	void MenuPressedInternal(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
	void MenuReleasedInternal(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
	void MenuSelectedInternal(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
	void MenuUnselectedInternal(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
	void MenuExpanded(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
	void MenuContracted(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
	void AppendMenuToGenlist(MenuItem* menu);

	GenlistEvent OnMenuScrollInternal;
	GenlistEvent OnMenuPressedInternal;
	GenlistEvent OnMenuReleasedInternal;
	GenlistEvent OnMenuSelectedInternal;
	GenlistEvent OnMenuUnselectedInternal;
	GenlistEvent OnMenuExpanded;
	GenlistEvent OnMenuContracted;

protected:
	virtual LIBAPI Evas_Object* CreateComponent(Evas_Object* root);

public:
	TreeMenu();
	void AddMenu(MenuItem* menu);
	void AddMenuAt(int index, MenuItem* menu);
	void RemoveMenu(MenuItem* menu);

	void AddMenu(const std::vector<MenuItem*>& listOfMenus);
	virtual ~TreeMenu();

	bool isClickPersist;
	void ResetCurrentlySelectedItem();

	typedef Event<TreeMenu*, MenuItem*> TreeMenuEvent;

	Property<TreeMenu, std::string>::Auto::ReadWrite IconEdjeFile;
	Property<TreeMenu, const std::vector<MenuItem*>&>::Get<&TreeMenu::GetMenuItems> MenuItems;
	Property<TreeMenu, bool>::GetSet<&TreeMenu::GetAutoExpanded, &TreeMenu::SetAutoExpanded> AutoExpanded;
	TreeMenuEvent OnMenuSelected;
};

}
}



#endif /* TREEMENU_H_ */
