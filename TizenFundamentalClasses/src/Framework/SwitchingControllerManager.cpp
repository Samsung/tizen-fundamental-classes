/*
 * SwitchingControllerManager.cpp
 *
 *  Created on: Feb 22, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Framework/Application.h"

using namespace TFC::Framework;

LIBAPI TFC::Framework::SwitchingControllerManager::SwitchingControllerManager(IAttachable* iattachable) :
	iattachable(iattachable)
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

void SwitchingControllerManager::NavigateTo(const char* controllerName, ObjectClass* data)
{
	NavigateTo(controllerName, data, false);
}

void SwitchingControllerManager::NavigateTo(const char* controllerName, ObjectClass* data, bool noTrail)
{
	auto controllerToSwitch = GetController(controllerName);

	controllerToSwitch->Unload();
	iattachable->Detach();
	iattachable->Attach(controllerToSwitch->View);
	controllerToSwitch->Load(data);

	currentController = controllerToSwitch;
}

bool SwitchingControllerManager::NavigateBack()
{
	return true;
}


LIBAPI
ControllerBase& TFC::Framework::SwitchingControllerManager::GetCurrentController() const
{
	return this->currentController;
}


LIBAPI
void TFC::Framework::SwitchingControllerManager::ClearNavigationHistory()
{
	// TODO implement this on Switching Controller Manager
}
