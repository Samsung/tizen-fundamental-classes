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
 *    Framework/MVCApplication.h
 *
 * Base class for TFC application
 *
 * Created on: 	Dec 13, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef TFC_FRAMEWORK_MVCAPPLICATION_H_
#define TFC_FRAMEWORK_MVCAPPLICATION_H_

#include "TFC/Framework/Application.h"
#include "TFC/Framework/ControllerManager.h"

namespace TFC {
namespace Framework {

class LIBAPI MVCApplicationBase:
	public UIApplicationBase,
	public StackingControllerManager
{
private:
	std::string mainController;
public:
	MVCApplicationBase(char const* appPackage);
	MVCApplicationBase(char const* appPackage, char const* mainController);
	virtual bool OnBackButtonPressed() final;
	virtual void OnApplicationCreated();
	virtual ~MVCApplicationBase();
};

}}



#endif /* TFC_FRAMEWORK_MVCAPPLICATION_H_ */
