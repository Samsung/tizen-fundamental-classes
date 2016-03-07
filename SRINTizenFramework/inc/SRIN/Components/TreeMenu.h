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

namespace SRIN { namespace Components {
	class LIBAPI MenuItem : public EventClass
	{
	private:
		std::vector<MenuItem*> subMenus;
		std::string menuIcon;
		void* itemData;
		Elm_Object_Item* genlistItem;
		bool expanded;
	public:
		MenuItem(std::string menuText, std::string menuIcon, void* itemData = nullptr);
		void AddSubMenu(MenuItem* subMenu);
		void RemoveSubMenu(int index);
		const std::vector<MenuItem*>& GetSubMenus() const;

		SimpleReadOnlyProperty<MenuItem, std::string> MenuText;

		template<class T>
		T* GetItemData();

		friend class TreeMenu;
		Event<MenuItem*, Elm_Object_Item*> OnMenuItemClick;
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

		typedef Event<Evas_Object*, Elm_Object_Item*> GenlistEvent;

		void MenuSelectedInternal(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
		void MenuExpanded(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);
		void MenuContracted(GenlistEvent* eventSource, Evas_Object* objSource, Elm_Object_Item* eventData);

		GenlistEvent OnMenuSelectedInternal;
		GenlistEvent OnMenuExpanded;
		GenlistEvent OnMenuContracted;

	protected:
		virtual LIBAPI Evas_Object* CreateComponent(Evas_Object* root);

	public:
		TreeMenu();
		void AddMenu(MenuItem* menu);
		void AddMenu(const std::vector<MenuItem*>& listOfMenus);
		virtual ~TreeMenu();

		typedef Event<TreeMenu*, MenuItem*> TreeMenuEvent;

		TreeMenuEvent OnMenuSelected;
	};

}}

template<class T>
T* SRIN::Components::MenuItem::GetItemData()
{
	return reinterpret_cast<T*>(itemData);
}

#endif /* TREEMENU_H_ */
