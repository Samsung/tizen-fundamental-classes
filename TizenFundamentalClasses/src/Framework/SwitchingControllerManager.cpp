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
 *    Framework/SwitchingControllerManager.cpp
 *
 * Created on:  Feb 22, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Framework/Application.h"
#include "TFC/Framework/ControllerManager.h"

using namespace TFC::Framework;

LIBAPI TFC::Framework::SwitchingControllerManager::SwitchingControllerManager()
{
}

LIBAPI ControllerBase* SwitchingControllerManager::GetController(char const* controllerName)
{
	//TODO Reimplement SwitchingControllerBase with localized storage of controller instance
	/*
	auto entry = this->GetControllerFactoryEntry(controllerName);

	if (entry->attachedData == nullptr)
		entry->attachedData = this->Instantiate(this);
	 */
	return nullptr;//static_cast<ControllerBase*>(entry->attachedData);
}



LIBAPI
ControllerBase& TFC::Framework::SwitchingControllerManager::GetCurrentController() const
{
	return *(this->currentController);
}


