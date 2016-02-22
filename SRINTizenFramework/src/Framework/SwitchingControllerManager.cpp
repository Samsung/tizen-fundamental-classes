/*
 * SwitchingControllerManager.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: gilang
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

	if(entry->attachedData == nullptr)
		entry->attachedData = entry->factoryMethod(this);

	return static_cast<ControllerBase*>(entry->attachedData);
}

LIBAPI void SwitchingControllerManager::SwitchTo(CString controllerName)
{
	auto controllerToSwitch = GetController(controllerName);

	controllerToSwitch->Unload();
	iattachable->Detach();
	iattachable->Attach(controllerToSwitch->View);
	controllerToSwitch->Load(nullptr);

	CurrentController = controllerToSwitch;
}
