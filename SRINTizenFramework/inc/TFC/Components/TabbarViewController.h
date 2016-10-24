/*
 * TabbarControllerBase.h
 *
 *  Created on: Feb 19, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef TFCFW_TABBARVIEWCONTROLLER_H_
#define TFCFW_TABBARVIEWCONTROLLER_H_

#include "TFC/Framework/Application.h"
#include <vector>
#include <string>

namespace TFC {
namespace Components {

struct TabEntry
{
	Framework::ControllerBase* controller;
	std::string tabText;
	Elm_Object_Item* objectItem;
	Evas_Object* content;
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
	Evas_Object* tabbar;

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
	TabbarViewController(TFC::Framework::ControllerManager* m, CString controllerName);
};
}
}

#endif /* TFCFW_TABBARVIEWCONTROLLER_H_ */
