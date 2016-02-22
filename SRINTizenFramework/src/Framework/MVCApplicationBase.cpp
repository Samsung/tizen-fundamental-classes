/*
 * MVCApplicationBase.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: gilang
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

LIBAPI MVCApplicationBase::MVCApplicationBase(CString appPackage, CString mainController) :
		ApplicationBase(appPackage),
		StackingControllerManager(this),
		mainController(mainController)
{
}

LIBAPI bool MVCApplicationBase::OnBackButtonPressed()
{
	return !this->NavigateBack();
}

LIBAPI void MVCApplicationBase::OnApplicationCreated()
{
	this->NavigateTo(mainController, nullptr);
}



LIBAPI MVCApplicationBase::~MVCApplicationBase()
{
}
