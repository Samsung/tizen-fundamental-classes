/*
 * MVCApplicationBase.cpp
 *
 *  Created on: Feb 15, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Framework/Application.h"
#include "TFC/Framework/MVCApplication.h"

using namespace TFC::Framework;

LIBAPI MVCApplicationBase::MVCApplicationBase(char const* appPackage, char const* mainController) :
	UIApplicationBase(appPackage), mainController(mainController)
{
}

LIBAPI bool MVCApplicationBase::OnBackButtonPressed()
{
	return !this->NavigateBack();
}

LIBAPI void MVCApplicationBase::OnApplicationCreated()
{
	this->SetApplicationContent(this->CreateViewContainer(this->GetApplicationConformant()));
	this->NavigateTo(mainController, nullptr);
}

LIBAPI MVCApplicationBase::~MVCApplicationBase()
{
}
