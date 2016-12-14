/*
 * FloatingMenu.h
 *
 *  Created on: May 20, 2016
 *      Contributor:
 *        Kevin Winata (k.winata@samsung.com)
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef FLOATINGMENU_H_
#define FLOATINGMENU_H_

#include "TFC/Components/ComponentBase.h"
#include "TFC/Components/BackButtonHandler.h"
#include "TFC/Components/MenuItem.h"

#include <vector>

/**
 * Component that provides a floating context menu.
 * Similar to FloatingButton, it uses Eext_FloatingButton.
 */
namespace TFC {
namespace Components {

class LIBAPI FloatingMenu :
		public BackButtonHandler,
		public ComponentBase,
		public EFL::EFLProxyClass,
		PropertyClass<FloatingMenu>
{
	using PropertyClass<FloatingMenu>::Property;
public:
	struct ContextMenuPackage
	{
		FloatingMenu* thisRef;
		MenuItem* menuItemRef;
		void RaiseEvent();
	};
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
protected:
	/**
	 * Method overriden from ComponentBase, creates the UI elements of the component.
	 *
	 * @param root The root/parent given for this component.
	 *
	 * @return An Eext_FloatingButton widget.
	 */
	virtual Evas_Object* CreateComponent(Evas_Object* root) final;
private:
	std::string buttonImage;
	std::vector<ContextMenuPackage*> currentItemPackages;
	std::vector<MenuItem*> rootMenu;

	EvasSmartEvent eventButtonClicked;
	EvasSmartEvent eventContextMenuDismissed;

	Evas_Object* naviframe;
	Evas_Object* floatingLayout;
	Evas_Object* button;
	Evas_Object* contextMenu;

	friend struct ContextMenuPackage;

	bool menuShown;

	bool BackButtonClicked() override;

	void OnButtonClicked(Evas_Object* obj, void* eventData);
	void OnContextMenuDismissed(Evas_Object* obj, void* eventData);
	void ShowMenu();
	void HideMenu();

	void RaiseOnClickEvent(MenuItem* menuItemRef);

	std::string const& GetButtonImage() const { return buttonImage; }
	void SetButtonImage(std::string const& str) { buttonImage = str; }
public:

	/**
	 * Property string that will be used as path of the floating button's image.
	 */
	Property<std::string const&>::Get<&FloatingMenu::GetButtonImage>::Set<&FloatingMenu::SetButtonImage> ButtonImage;
};

}}



#endif /* FLOATINGMENU_H_ */
