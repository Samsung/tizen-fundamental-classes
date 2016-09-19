/*
 * SidebarView.h
 *
 *  Created on: Feb 22, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRINFW_SIDEBARVIEW_H_
#define SRINFW_SIDEBARVIEW_H_

#include "SRIN/Framework/Application.h"
#include "SRIN/EFL.h"
#include "SRIN/Components/BackButtonHandler.h"


namespace SRIN {
namespace Components {

/**
 * Subclass of ViewBase that can be used to implement multiple screen that has a sidebar panel.
 * Internally, it uses Elm_Panel which content can be set by overriding CreateSidebar() method.
 * By using this class, you can't use CreateView() like normal view, instead you can use Attach and Detach
 * to add, remove or switch the screen content.
 * It also provides customizable button on the naviframe that can be used to open or close the sidebar panel.
 */
class LIBAPI SidebarView: public Framework::ViewBase, public Framework::IAttachable, public Framework::INaviframeContent, public BackButtonHandler
{
private:

	Evas_Object* leftPanel;
	Evas_Object* background;
	Evas_Object* currentContent;
	Evas_Object* bg;
	Evas_Object* contentWrapper;

	EFL::EvasSmartEvent drawerButtonClick;
	EFL::EvasSmartEvent drawerScroll;

	void OnDrawerButtonClick(EFL::EvasSmartEvent* eventSource, Evas_Object* objSource, void* eventData);
	void OnDrawerScrolling(EFL::EvasSmartEvent* eventSource, Evas_Object* objSource, void* eventData);
	void OnDrawerOpened(Event<SidebarView*, void*>* event, SidebarView* sidebarView, void* unused);
	void OnDrawerClosed(Event<SidebarView*, void*>* event, SidebarView* sidebarView, void* unused);
protected:
	/**
	 * Layout of the view, created as child of the view root.
	 */
	Evas_Object* layout;

	virtual Evas_Object* CreateView(Evas_Object* root) final;

	/**
	 * Method that have to be overriden to set the content of the sidebar panel.
	 *
	 * @param root Same as the view root.
	 * @return Evas_Object representation of the content.
	 */
	virtual Evas_Object* CreateSidebar(Evas_Object* root) = 0;

	/**
	 * Method that can be overriden to set the style of the drawer button.
	 *
	 * @param button Drawer button.
	 */
	virtual void DrawerButtonStyle(Evas_Object* button);


	virtual Evas_Object* CreateContentWrapper(Evas_Object* root);

	/**
	 * Method from BackButtonHandler that will be called when the back button is pressed.
	 * The default implementation of SidebarView is to close the sidebar panel if it's opened.
	 *
	 * @return true if the application should be closed after clicking back.
	 * 		   false to cancel closing the application.
	 */
	virtual bool BackButtonClicked();

public:
	/**
	 * Constructor of SidebarView.
	 */
	SidebarView();

	/**
	 * Method that can be called to add or switch the content of the screen.
	 * Can be overriden to create a custom implementation.
	 *
	 * @param view ViewBase object pointer that you want to display.
	 */
	virtual void Attach(ViewBase* view);

	/**
	 * Method that can be called to remove the content of the screen.
	 */
	virtual void Detach();

	/**
	 * Method to toggle the sidebar panel open or close.
	 */
	void ToggleSidebar();

	/**
	 * Method to force the sidebar panel open or close.
	 *
	 * @param show If true, it will open the sidebar panel. If false then it will close the sidebar panel.
	 */
	void ToggleSidebar(bool show);

	virtual Evas_Object* GetTitleLeftButton(CString* buttonPart);
	virtual Evas_Object* GetTitleRightButton(CString* buttonPart);
	virtual CString GetContentStyle();

	typedef void (*DrawerButtonStyleFunc)(Evas_Object* btn);

	/**
	 * Event that will be triggered when the sidebar panel is opened.
	 */
	Event<SidebarView*, void*> DrawerOpened;

	/**
	 * Event that will be triggered when the sidebar panel is closed.
	 */
	Event<SidebarView*, void*> DrawerClosed;

	/**
	 * Method to get whether the sidebar panel is opened or closed.
	 *
	 * @return EINA_TRUE if opened, EINA_FALSE if closed.
	 */
	Eina_Bool IsDrawerOpened();
};



}
}

#endif /* SIDEBARVIEW_H_ */
