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
	virtual Evas_Object* CreateComponent(Evas_Object* root);
	void SetText(const std::string& text);
	std::string& GetText();

public:


	ContextMenu();
	void AddMenu(MenuItem* menu);
	void AddMenuAt(int index, MenuItem* menu);
	void RemoveMenu(MenuItem* menu);

	void AddMenu(const std::vector<MenuItem*>& listOfMenus);
	void SetMenu(const std::vector<MenuItem*>& listOfMenus);
	virtual ~ContextMenu();
	Property<ContextMenu, std::string&>::GetSet<&ContextMenu::GetText, &ContextMenu::SetText> Text;
};

}
}



#endif /* CONTEXTMENU_H_ */
