/*
 * SwitchingControllerManager.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

LIBAPI SRIN::Framework::SwitchingControllerManager::SwitchingControllerManager(IAttachable* iattachable) :
	iattachable(iattachable)
{
}

LIBAPI ControllerBase* SwitchingControllerManager::GetController(CString controllerName)
{
	auto entry = this->GetControllerFactoryEntry(controllerName);

	if (entry->attachedData == nullptr)
		entry->attachedData = entry->factoryMethod(this);

	return static_cast<ControllerBase*>(entry->attachedData);
}

void SwitchingControllerManager::NavigateTo(const char* controllerName, void* data)
{
	NavigateTo(controllerName, data, false);
}

void SwitchingControllerManager::NavigateTo(const char* controllerName, void* data, bool noTrail)
{
	auto controllerToSwitch = GetController(controllerName);

	controllerToSwitch->Unload();
	iattachable->Detach();
	iattachable->Attach(controllerToSwitch->View);
	controllerToSwitch->Load(data);

	CurrentController = controllerToSwitch;
}

bool SwitchingControllerManager::NavigateBack()
{
	return true;
}
