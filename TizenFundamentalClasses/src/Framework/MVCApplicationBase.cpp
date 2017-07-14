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
 *    Framework/ControllerManager.cpp
 *
 * Created on:  Feb 15, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Framework/Application.h"
#include "TFC/Framework/MVCApplication.h"

using namespace TFC::Framework;

LIBAPI MVCApplicationBase::MVCApplicationBase(char const* appPackage, char const* mainController) :
	UIApplicationBase(appPackage), mainController(mainController)
{
}

LIBAPI MVCApplicationBase::MVCApplicationBase(char const* appPackage) :
	UIApplicationBase(appPackage)
{
}


LIBAPI bool MVCApplicationBase::OnBackButtonPressed()
{
	try
	{
		return !this->NavigateBack();
	}
	catch(TFC::TFCException const& ex)
	{
		return false;
	}
}

LIBAPI void MVCApplicationBase::OnApplicationCreated()
{
	this->SetApplicationContent(this->CreateViewContainer(this->GetApplicationConformant()));

	if(!mainController.empty())
		this->NavigateTo(mainController.c_str(), nullptr);
}

LIBAPI MVCApplicationBase::~MVCApplicationBase()
{

}
