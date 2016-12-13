/*
 * SwitchingControllerManager.cpp
 *
 *  Created on: Feb 22, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
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


