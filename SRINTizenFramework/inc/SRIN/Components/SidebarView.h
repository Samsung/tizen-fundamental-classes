/*
 * SidebarView.h
 *
 *  Created on: Feb 22, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SIDEBARVIEW_H_
#define SIDEBARVIEW_H_

#include "SRIN/Framework/Application.h"

namespace SRIN { namespace Components {
	class LIBAPI SidebarView: public Framework::ViewBase, public Framework::IAttachable, public Framework::INaviframeContent
	{
	private:
		Evas_Object* layout;
		Evas_Object* leftPanel;
		Evas_Object* background;
		Evas_Object* currentContent;

		ElementaryEvent drawerButtonClick;

		void OnDrawerButtonClick(ElementaryEvent* eventSource, Evas_Object* objSource, void* eventData);
	protected:
		virtual Evas_Object* CreateView(Evas_Object* root);
		virtual Evas_Object* CreateSidebar(Evas_Object* root) = 0;
		virtual void DrawerButtonStyle(Evas_Object* button);
	public:
		SidebarView();
		virtual void Attach(ViewBase* view);
		virtual void Detach();
		virtual Evas_Object* GetTitleLeftButton(CString* buttonPart) final;
		virtual Evas_Object* GetTitleRightButton(CString* buttonPart) final;
		virtual CString GetContentStyle();

		typedef void (*DrawerButtonStyleFunc)(Evas_Object* btn);

	};
}}

#endif /* SIDEBARVIEW_H_ */
