/*
 * FloatingMenu.h
 *
 *  Created on: May 20, 2016
 *      Author: Kevin Winata
 */

#ifndef FLOATINGMENU_H_
#define FLOATINGMENU_H_

#include "SRIN/Components/ComponentBase.h"
#include "SRIN/Components/BackButtonHandler.h"
#include "SRIN/Components/MenuItem.h"

#include <vector>

/**
 * Component that provides a floating context menu.
 * Similar to FloatingButton, it uses Eext_FloatingButton.
 */
namespace SRIN {
	namespace Components {
		class LIBAPI FloatingMenu : public ComponentBase, BackButtonHandler {
		public:
			struct ContextMenuPackage
			{
				FloatingMenu* thisRef;
				MenuItem* menuItemRef;
				void RaiseEvent();
			};
		private:
			Evas_Object* naviframe;
			Evas_Object* floatingLayout;
			Evas_Object* button;
			Evas_Object* contextMenu;

			EFL::EvasSmartEvent eventButtonClicked;
			EFL::EvasSmartEvent eventContextMenuDismissed;

			friend struct ContextMenuPackage;
			std::vector<ContextMenuPackage*> currentItemPackages;

			bool menuShown;
			std::vector<MenuItem*> rootMenu;

			void OnButtonClicked(EFL::EvasSmartEvent* ev, Evas_Object* obj, void* eventData);
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
			 * @return An Eext_FloatingButton widget.
			 */
			virtual Evas_Object* CreateComponent(Evas_Object* root) final;
		public:
			/**
			 * Constructor of FloatingMenu.
			 *
			 * @note This component requires a swallow named "elm.swallow.floatingbutton"
			 * 		 that must be present in the layout of views that use it.
			 */
			FloatingMenu();

			/**
			 * Destructor of FloatingMenu.
			 */
			virtual ~FloatingMenu();

			/**
			 * String that will be used as path of the floating button's image.
			 */
			std::string buttonImage;

			/**
			 * Method to add MenuItem to the floating menu.
			 *
			 * @param menu MenuItem that will be added.
			 */
			void AddMenu(MenuItem* menu);

			/**
			 * Method to add MenuItem at a specific position to the floating menu.
			 *
			 * @param index Index that indicates where the item will be placed.
			 * @param menu MenuItem that will be added.
			 */
			void AddMenuAt(int index, MenuItem* menu);

			/**
			 * Method to remove a particular MenuItem from the floating menu.
			 *
			 * @param menu MenuItem that will be removed.
			 */
			void RemoveMenu(MenuItem* menu);

			/**
			 * Method to bulk-add MenuItems to the floating menu.
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
		};
	}
}



#endif /* FLOATINGMENU_H_ */
