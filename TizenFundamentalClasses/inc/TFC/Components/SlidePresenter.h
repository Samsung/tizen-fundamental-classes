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
 *    Components/SlidePresenter.h
 *
 * Component to provide slider component which may contains another widget
 * inside it
 *
 * Created on:  Sep 13, 2016
 * Author:      Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef SLIDEPRESENTER_H_
#define SLIDEPRESENTER_H_

#include "TFC/Components/ComponentBase.h"

#include <vector>

namespace TFC {
namespace Components {

class LIBAPI SlidePresenter :
		public ComponentBase,
		public EFL::EFLProxyClass,
		public EventEmitterClass<SlidePresenter>
{
using EventEmitterClass<SlidePresenter>::Event;
public:
	SlidePresenter();
	virtual ~SlidePresenter();

	void InsertPage(Evas_Object* page);
	void InsertPageAt(Evas_Object* page, int index);
	void Remove(int index);

	Event<int> eventPageChanged;
protected:
	virtual Evas_Object* CreateComponent(Evas_Object* root) override;
private:
	std::vector<Evas_Object*> pageList;

	Evas_Object* box;
	Evas_Object* scroller;
	Evas_Object* index;

	EvasObjectEvent eventLayoutResizeInternal;
	EvasSmartEvent eventPageScrolledInternal;
	EvasSmartEvent eventIndexSelectedInternal;

	void OnLayoutResize(EFL::EvasEventSourceInfo objSource, void* event_data);
	void OnPageScrolled(Evas_Object* source, void* event_data);
	void OnIndexSelected(Evas_Object* source, void* event_data);
};


}
}



#endif /* SLIDEPRESENTER_H_ */
