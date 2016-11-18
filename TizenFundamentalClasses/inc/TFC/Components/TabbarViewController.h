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
		public EFL::EFLProxyClass,
		public Framework::ViewBase
{
private:
	std::vector<TabEntry> tabs;
	bool disableChangeTabByScroll;
	int currentTab;

	Evas_Object* scroller;
	Evas_Object* box;
	Evas_Object* tabbar;

	EvasObjectEvent eventLayoutResize;
	EvasSmartEvent eventTabContentScrolled;

	void OnLayoutResize(EFL::EvasEventSourceInfo objSource, void* event_data);
	void OnTabContentScrolled(Evas_Object* source, void* event_data);
	void LookupAndBringContent(Elm_Object_Item* tabItem);

	//EvasSmartEvent eventTabbarButtonClicked;
	//void OnTabbarButtonClicked(EvasSmartEvent::Type* event, Evas_Object* source, void* event_data);
	static void OnTabbarButtonClicked(void *data, Evas_Object *source, void *event_data);

protected:
	void AddTab(std::string text, Framework::ControllerBase& controller);
	virtual Evas_Object* CreateView(Evas_Object* root) final;

public:
	TabbarViewController(TFC::Framework::ControllerManager* m, char const* controllerName);
};
}
}

#endif /* TFCFW_TABBARVIEWCONTROLLER_H_ */
