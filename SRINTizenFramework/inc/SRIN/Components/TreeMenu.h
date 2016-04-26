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

namespace SRIN {
namespace Components {

class MenuItem;

class LIBAPI CustomMenuStyle
{
private:
	Elm_Genlist_Item_Class* customStyle;

	operator Elm_Genlist_Item_Class* ();
public:
	CustomMenuStyle(CString style);
	virtual ~CustomMenuStyle();
	virtual std::string GetString(MenuItem* data, Evas_Object *obj, const char *part) = 0;
	virtual Evas_Object* GetContent(MenuItem* data, Evas_Object *obj, const char *part) = 0;

	friend class TreeMenu;
};

class LIBAPI MenuItem: public EventClass, public PropertyClass
{
private:
	std::vector<MenuItem*> subMenus;
	void* itemData;
	Elm_Object_Item* genlistItem;
	bool expanded;
public:
	MenuItem(std::string menuText, std::string menuIcon, void* itemData = nullptr, CustomMenuStyle* customStyle = nullptr);
	void AddSubMenu(MenuItem* subMenu);
	void RemoveSubMenu(int index);
	const std::vector<MenuItem*>& GetSubMenus() const;

	Property<MenuItem, std::string>::Auto::ReadOnly Text;
	Property<MenuItem, std::string>::Auto::ReadOnly MenuIcon;
	Property<MenuItem, CustomMenuStyle*>::Auto::ReadOnly CustomItemStyle;

	template<class T>
	T* GetItemData();

	friend class TreeMenu;
	Event<MenuItem*, Elm_Object_Item*> OnMenuItemClick;
};

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

	typedef Event<TreeMenu*, MenuItem*> TreeMenuEvent;

	Property<TreeMenu, std::string>::Auto::ReadWrite IconEdjeFile;
	Property<TreeMenu, const std::vector<MenuItem*>&>::Get<&TreeMenu::GetMenuItems> MenuItems;
	Property<TreeMenu, bool>::GetSet<&TreeMenu::GetAutoExpanded, &TreeMenu::SetAutoExpanded> AutoExpanded;
	TreeMenuEvent OnMenuSelected;
};

}
}

template<class T>
T* SRIN::Components::MenuItem::GetItemData()
{
	return reinterpret_cast<T*>(itemData);
}

#endif /* TREEMENU_H_ */
