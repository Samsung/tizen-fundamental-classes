/*
 * SidebarView.h
 *
 *  Created on: Feb 22, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRINFW_SIDEBARVIEW_H_
#define SRINFW_SIDEBARVIEW_H_

#include "SRIN/Framework/Application.h"
#include "SRIN/Components/BackButtonHandler.h"

namespace SRIN {
namespace Components {


class LIBAPI SidebarView: public Framework::ViewBase, public Framework::IAttachable, public Framework::INaviframeContent, public BackButtonHandler
{
private:

	Evas_Object* leftPanel;
	Evas_Object* background;
	Evas_Object* currentContent;
	Evas_Object* bg;

	ElementaryEvent drawerButtonClick;
	ElementaryEvent drawerScroll;

	void OnDrawerButtonClick(ElementaryEvent* eventSource, Evas_Object* objSource, void* eventData);
	void OnDrawerScrolling(ElementaryEvent* eventSource, Evas_Object* objSource, void* eventData);
	void OnDrawerOpened(Event<SidebarView*, void*>* event, SidebarView* sidebarView, void* unused);
	void OnDrawerClosed(Event<SidebarView*, void*>* event, SidebarView* sidebarView, void* unused);
protected:
	Evas_Object* layout;
	virtual Evas_Object* CreateView(Evas_Object* root) final;
	virtual Evas_Object* CreateSidebar(Evas_Object* root) = 0;
	virtual void DrawerButtonStyle(Evas_Object* button);
	virtual bool BackButtonClicked();
public:
	SidebarView();
	virtual void Attach(ViewBase* view);
	virtual void Detach();
	void ToggleSidebar();
	void ToggleSidebar(bool show);
	virtual Evas_Object* GetTitleLeftButton(CString* buttonPart);
	virtual Evas_Object* GetTitleRightButton(CString* buttonPart);
	virtual CString GetContentStyle();

	typedef void (*DrawerButtonStyleFunc)(Evas_Object* btn);

	Event<SidebarView*, void*> DrawerOpened;
	Event<SidebarView*, void*> DrawerClosed;
	Eina_Bool IsDrawerOpened();
};



}
}

#endif /* SIDEBARVIEW_H_ */
