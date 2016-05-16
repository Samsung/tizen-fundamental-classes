/*
 * MenuItem.h
 *
 *  Created on: Apr 26, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_COMPONENTS_MENUITEM_H_
#define SRIN_COMPONENTS_MENUITEM_H_

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
	Elm_Object_Item* GetGenlistItem();

	friend class TreeMenu;
	Event<MenuItem*, Elm_Object_Item*> OnMenuItemClick;
};

}
}

template<class T>
T* SRIN::Components::MenuItem::GetItemData()
{
	return reinterpret_cast<T*>(itemData);
}

#endif /* MENUITEM_H_ */
