/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Components/TabbarControllerBase.h
 *
 * A class which acts as view and controller altogether which provides a
 * tab based UI, and can contains another view-controller object internally,
 * as well as performing navigation between tabs
 *
 * Created on:  Feb 19, 2016
 * Author:      Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
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
public:
	TabbarViewController(TFC::Framework::ControllerManager* m, char const* controllerName);
protected:
	void AddTab(std::string text, Framework::ControllerBase& controller);
	virtual Evas_Object* CreateView(Evas_Object* root) final;
	void OnReload(TFC::ObjectClass* param) override;
private:
	std::vector<TabEntry> tabs;
	int currentTab;

	EvasObjectEvent eventLayoutResize;
	EvasSmartEvent eventTabContentScrolled;

	Evas_Object* scroller;
	Evas_Object* box;
	Evas_Object* tabbar;

	bool disableChangeTabByScroll;

	void OnLayoutResize(EFL::EvasEventSourceInfo objSource, void* event_data);
	void OnTabContentScrolled(Evas_Object* source, void* event_data);
	void LookupAndBringContent(Elm_Object_Item* tabItem);

	//EvasSmartEvent eventTabbarButtonClicked;
	//void OnTabbarButtonClicked(EvasSmartEvent::Type* event, Evas_Object* source, void* event_data);
	static void OnTabbarButtonClicked(void *data, Evas_Object *source, void *event_data);

};
}
}

#endif /* TFCFW_TABBARVIEWCONTROLLER_H_ */
