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

			ElementaryEvent eventButtonClicked;
			ElementaryEvent eventContextMenuDismissed;

			friend struct ContextMenuPackage;
			std::vector<ContextMenuPackage*> currentItemPackages;

			bool menuShown;
			std::vector<MenuItem*> rootMenu;

			void OnButtonClicked(ElementaryEvent* ev, Evas_Object* obj, void* eventData);
			void OnContextMenuDismissed(ElementaryEvent* ev, Evas_Object* obj, void* eventData);
			bool BackButtonClicked();
			void ShowMenu();
			void HideMenu();

			void RaiseOnClickEvent(MenuItem* menuItemRef);
		protected:
			virtual Evas_Object* CreateComponent(Evas_Object* root) final;
		public:
			FloatingMenu();
			virtual ~FloatingMenu();
			std::string buttonImage;

			void AddMenu(MenuItem* menu);
			void AddMenuAt(int index, MenuItem* menu);
			void RemoveMenu(MenuItem* menu);

			void AddMenu(const std::vector<MenuItem*>& listOfMenus);
			void SetMenu(const std::vector<MenuItem*>& listOfMenus);
		};
	}
}



#endif /* FLOATINGMENU_H_ */
