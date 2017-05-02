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
 *    Framework/ControllerManager.h
 *
 * Class which manages the instance of controller in an application
 *
 * Created on: 	Dec 13, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef TFC_FRAMEWORK_CONTROLLERMANAGER_H_
#define TFC_FRAMEWORK_CONTROLLERMANAGER_H_

#include "TFC/Core.h"

namespace TFC {
namespace Framework {

class LIBAPI StackingControllerManager: public ControllerManager
{
private:
	std::deque<std::unique_ptr<ControllerBase>> controllerStack;

	Evas_Object* viewContainer;

	void PushController(ControllerBase* controller);
	bool PopController();

	void PushView(ViewBase* view);
	void PopView();

	void DoNavigateBackward();
	void DoNavigateForward(char const* targetControllerName, ObjectClass* data, bool noTrail);
protected:
	virtual ControllerBase& GetCurrentController() const override;
	virtual void PerformNavigation(char const* controllerName, ObjectClass* data, NavigationFlag mode) override;
	virtual Evas_Object* CreateViewContainer(Evas_Object* parent) override;
public:
	/**
	 * Constructor of StackingControllerManager
	 *
	 * @param app IAttachable which this controller manager will attach the underlying view
	 */
	StackingControllerManager();
};

class SwitchingControllerManager: public ControllerManager
{
private:
	ControllerBase* GetController(char const* controllerName);
	ControllerBase* currentController;
protected:
	virtual ControllerBase& GetCurrentController() const override;
public:
	SwitchingControllerManager();
	//void SwitchTo(char const* controllerName);
};



}}



#endif /* TFC_FRAMEWORK_CONTROLLERMANAGER_H_ */
