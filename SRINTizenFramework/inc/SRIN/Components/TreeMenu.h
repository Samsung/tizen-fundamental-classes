/*
 * TreeMenu.h
 *
 *  Created on: Feb 23, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef TREEMENU_H_
#define TREEMENU_H_

#include <vector>
#include <string>

#include "SRIN/Components/ComponentBase.h"

namespace SRIN { namespace Components {
	class LIBAPI MenuItem : public EventClass
	{
	private:
		std::vector<MenuItem*> subMenus;
		std::string menuIcon;
		std::string menuText;
		void* itemData;
		Elm_Object_Item* genlistItem;
		bool expanded;
	public:
		MenuItem(std::string menuText, std::string menuIcon);
		void AddSubMenu(MenuItem* subMenu);
		void RemoveSubMenu(int index);
		const std::vector<MenuItem*>& GetSubMenus() const;


		friend class TreeMenu;
		Event OnMenuItemClick;
	};

	class LIBAPI TreeMenu : virtual public ComponentBase
	{
	private:
		Evas_Object* genlist;
		Elm_Genlist_Item_Class* itemClass;
		Elm_Genlist_Item_Class* submenuItemClass;


		std::vector<MenuItem*> rootMenu;

		void GenerateRootMenu();
		void GenerateSubMenu(MenuItem* subMenu);

		void MenuSelected(Event* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
		void MenuExpanded(Event* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
		void MenuContracted(Event* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);

		Event OnMenuSelected;
		Event OnMenuExpanded;
		Event OnMenuContracted;

	protected:
		virtual LIBAPI Evas_Object* CreateComponent(Evas_Object* root);
	public:
		TreeMenu();
		void AddMenu(MenuItem* menu);
		virtual ~TreeMenu();
	};

}}



#endif /* TREEMENU_H_ */
