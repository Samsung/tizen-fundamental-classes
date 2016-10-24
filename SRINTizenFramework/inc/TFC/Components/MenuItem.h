/*
 * MenuItem.h
 *
 *  Created on: Apr 26, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#ifndef TFC_COMPONENTS_MENUITEM_H_
#define TFC_COMPONENTS_MENUITEM_H_

#include <vector>
#include <string>

#include "TFC/Components/ComponentBase.h"

namespace TFC {
namespace Components {

class MenuItem;

/**
 * Class that can be inherited to provide custom style and content for MenuItem.
 */
class LIBAPI CustomMenuStyle
{
private:
	Elm_Genlist_Item_Class* customStyle;

	operator Elm_Genlist_Item_Class* ();
public:
	/**
	 * Constructor for CustomMenuStyle.
	 *
	 * @param style Name of the style.
	 */
	CustomMenuStyle(CString style);

	/**
	 * Destructor for CustomMenuStyle.
	 */
	virtual ~CustomMenuStyle();

	/**
	 * Abstract method for providing string representation from a part of a menu item.
	 *
	 * @param data Menu item's data.
	 * @param obj Parent Evas_Object.
	 * @param part Corresponding part's name.
	 *
	 * @return The string representation.
	 */
	virtual std::string GetString(MenuItem* data, Evas_Object *obj, const char *part) = 0;

	/**
	 * Abstract method for providing Evas_Object representation from a part of a menu item.
	 *
	 * @param data Menu item's data.
	 * @param obj Parent Evas_Object.
	 * @param part Corresponding part's name.
	 *
	 * @return The Evas_Object representation.
	 */
	virtual Evas_Object* GetContent(MenuItem* data, Evas_Object *obj, const char *part) = 0;

	friend class TreeMenu;
};

/**
 * Class that represents an item inside a tree menu.
 * @see TreeMenu
 */
class LIBAPI MenuItem: public EventClass, public PropertyClass
{
private:
	std::vector<MenuItem*> subMenus;
	MenuItem* parentMenu;
	void* itemData;
	Elm_Object_Item* genlistItem;
	bool expanded;
public:
	/**
	 * Constructor of MenuItem.
	 *
	 * @param menuText String that will be displayed in the menu.
	 * @param menuIcon Path to the icon image file that will be displayed.
	 * @param itemData Arbitary data that the item has.
	 * @param customStyle Pointer to a CustomMenuStyle object that can be used to override the default style and content.
	 */
	MenuItem(std::string menuText, std::string menuIcon, void* itemData = nullptr, CustomMenuStyle* customStyle = nullptr);

	/**
	 * Method to add submenu items to a parent item.
	 *
	 * @param subMenu MenuItem object that will be added as a submenu.
	 */
	void AddSubMenu(MenuItem* subMenu);

	/**
	 * Method to remove a submenu from its parent.
	 *
	 * @param index Index of the submenu that will be removed. Note that last added submenu will have the biggest index.
	 */
	void RemoveSubMenu(int index);

	/**
	 * Method to get all submenus.
	 *
	 * @return Reference to the submenus vector. Note that functions marked as non-const will not work on the returned reference.
	 */
	const std::vector<MenuItem*>& GetSubMenus() const;

	/**
	 * Method to get the parent MenuItem of an item.
	 *
	 * @return Pointer to parent item. If the item doesn't have any parent, it will return nullptr.
	 */
	MenuItem* GetParentMenu() const;

	/**
	 * Method to set or change the parent MenuItem of an item.
	 *
	 * @param parentMenu MenuItem that will be set as the parent.
	 */
	void SetParentMenu(MenuItem* parentMenu);

	/**
	 * Property that enables getting & setting text of the menu item.
	 * The return/parameter type is string.
	 */
	Property<MenuItem, std::string>::Auto::ReadOnly Text;

	/**
	 * Property that enables getting & setting icon path of the menu item.
	 * The return/parameter type is string.
	 */
	Property<MenuItem, std::string>::Auto::ReadOnly MenuIcon;

	/**
	 * Property that enables getting & setting custom style of the menu item.
	 * The return/parameter type is CustomMenuStyle.
	 */
	Property<MenuItem, CustomMenuStyle*>::Auto::ReadOnly CustomItemStyle;

	template<class T>
	T* GetItemData();
	Elm_Object_Item* GetGenlistItem();

	friend class TreeMenu;
	Event<MenuItem*, Elm_Object_Item*> OnMenuItemClick;
};

}
}

template<class T>
T* TFC::Components::MenuItem::GetItemData()
{
	return reinterpret_cast<T*>(itemData);
}

#endif /* MENUITEM_H_ */
