/*
 * SidebarView.h
 *
 *  Created on: Feb 22, 2016
 *      Author: gilang
 */

#ifndef SIDEBARVIEW_H_
#define SIDEBARVIEW_H_

#include "SRIN/Framework/Application.h"

namespace SRIN { namespace Components {
	class SidebarView: public Framework::ViewBase, public Framework::IAttachable, public Framework::INaviframeContent
	{
	private:
		Evas_Object* layout;
		Evas_Object* leftPanel;
		Evas_Object* background;
		Evas_Object* currentContent;

		Event drawerButtonClick;
	protected:
		virtual Evas_Object* CreateView(Evas_Object* root);
		virtual Evas_Object* CreateSidebar(Evas_Object* root) = 0;
	public:
		SidebarView();
		virtual void Attach(ViewBase* view);
		virtual void Detach();
		virtual Evas_Object* GetTitleLeftButton();
		virtual Evas_Object* GetTitleRightButton();
		virtual CString GetContentStyle();
	};
}}

#endif /* SIDEBARVIEW_H_ */
