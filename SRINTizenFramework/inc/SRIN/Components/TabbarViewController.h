/*
 * TabbarControllerBase.h
 *
 *  Created on: Feb 19, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRINFW_TABBARVIEWCONTROLLER_H_
#define SRINFW_TABBARVIEWCONTROLLER_H_

#include "SRIN/Framework/Application.h"
#include <vector>
#include <string>

namespace SRIN {
namespace Components {

struct TabEntry
{
	Framework::ControllerBase* controller;
	std::string tabText;
	Elm_Object_Item* objectItem;
	int tabNumber;
};

class LIBAPI TabbarViewController:
		public Framework::ControllerBase,
		public Framework::ViewBase
{
private:
	std::vector<TabEntry> tabs;
	bool disableChangeTabByScroll;
	int currentTab;

	Evas_Object* scroller;
	Evas_Object* box;

	EFL::EvasObjectEvent eventLayoutResize;
	EFL::EvasSmartEvent eventTabbarButtonClicked;
	EFL::EvasSmartEvent eventTabContentScrolled;

	void OnLayoutResize(EFL::EvasObjectEvent* event, EFL::EvasEventSourceInfo* objSource, void* event_data);
	void OnTabbarButtonClicked(EFL::EvasSmartEvent* event, Evas_Object* source, void* event_data);
	void OnTabContentScrolled(EFL::EvasSmartEvent* event, Evas_Object* source, void* event_data);
	void LookupAndBringContent(Elm_Object_Item* tabItem);
protected:
	void AddTab(std::string text, Framework::ControllerBase& controller);
	virtual Evas_Object* CreateView(Evas_Object* root) final;
public:
	TabbarViewController(SRIN::Framework::ControllerManager* m, CString controllerName);
};
}
}

#endif /* SRINFW_TABBARVIEWCONTROLLER_H_ */
