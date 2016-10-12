/*
 * MVCApplicationBase.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: gilang
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

MVCApplicationBase::MVCApplicationBase(CString appPackage, CString mainController) :
		ControllerManager(this),
		ApplicationBase(appPackage),
		mainController(mainController)
{
}

bool MVCApplicationBase::OnBackButtonPressed()
{
	return !this->NavigateBack();
}

void MVCApplicationBase::OnApplicationCreated()
{
	this->NavigateTo(mainController, nullptr);
}



