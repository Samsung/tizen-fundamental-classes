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
};

class LIBAPI TabbarViewController:
		public Framework::ControllerBase,
		public Framework::ViewBase
{
private:
	std::vector<TabEntry> tabs;

	Evas_Object* scroller;
	Evas_Object* box;

	EFL::EvasObjectEvent eventLayoutResize;

	void OnLayoutResize(EFL::EvasObjectEvent* event, EFL::EvasEventSourceInfo* objSource, void* event_data);
protected:
	void AddTab(std::string text, Framework::ControllerBase& controller);
	virtual Evas_Object* CreateView(Evas_Object* root) final;
public:
	TabbarViewController(SRIN::Framework::ControllerManager* m, CString controllerName);
};
}
}

#endif /* SRINFW_TABBARVIEWCONTROLLER_H_ */
