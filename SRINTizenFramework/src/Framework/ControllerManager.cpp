/*
 * ControllerManager.cpp
 *
 *  Created on: Feb 15, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

void ControllerManager_FreeFactory(void* data)
{
	ControllerFactory* factory = static_cast<ControllerFactory*>(data);
	delete factory;
}

SRIN::Framework::ControllerManager::ControllerManager() :
	CurrentController(this)
{
	this->controllerTable = eina_hash_string_superfast_new(ControllerManager_FreeFactory);
}

LIBAPI void ControllerManager::RegisterControllerFactory(ControllerFactory* controller)
{
	void* entry = eina_hash_find(this->controllerTable, controller->controllerName);
	if (entry != 0)
	{
		abort();
	}
	else
	{
		eina_hash_add(this->controllerTable, controller->controllerName, controller);
	}
}

LIBAPI ControllerFactory* ControllerManager::GetControllerFactoryEntry(const char* controllerName)
{
	void* entry = eina_hash_find(this->controllerTable, controllerName);

	return static_cast<ControllerFactory*>(entry);
}

SRIN::Framework::ControllerManager::~ControllerManager()
{
}

LIBAPI StackingControllerManager::StackingControllerManager(IAttachable* app) :
	app(app)
{
	this->eventPerformNavigation += AddEventHandler(StackingControllerManager::OnPerformNavigation);
	this->chain = nullptr;
}

LIBAPI void StackingControllerManager::NavigateTo(const char* controllerName, ObjectClass* data)
{
	if(this->pendingNavigation)
		return; // TODO change this to exception

	this->pendingNavigation = true;
	this->navigateForward = true;
	this->nextControllerName = controllerName;
	this->data = data;
	this->noTrail = false;
	EFL::QueueJob(this->eventPerformNavigation);


}

void StackingControllerManager::PushController(ControllerBase* controller)
{
	ControllerChain* newChain = new ControllerChain();
	newChain->instance = controller;
	newChain->next = this->chain;
	this->chain = newChain;
}

bool StackingControllerManager::PopController()
{
	if (this->chain != nullptr)
	{
		ControllerChain* oldChain = this->chain;
		this->chain = oldChain->next;
		delete oldChain->instance;
		delete oldChain;
	}

	if (this->chain)
		return true;
	else
		return false;
}

LIBAPI bool StackingControllerManager::NavigateBack()
{
	this->pendingNavigation = true;
	this->navigateForward = false;
	EFL::QueueJob(this->eventPerformNavigation);

	// The new implementation should always return true
	// as the codes might interpret False to exit the application
	return true;
}

LIBAPI void StackingControllerManager::NavigateTo(const char* controllerName, ObjectClass* data, bool noTrail)
{
	if(this->pendingNavigation)
		return; // TODO change this to exception

	this->pendingNavigation = true;
	this->navigateForward = true;
	this->nextControllerName = controllerName;
	this->data = data;
	this->noTrail = noTrail;
	EFL::QueueJob(this->eventPerformNavigation);
}

void StackingControllerManager::OnPerformNavigation(EFL::EcoreJobEvent* ev, void* u1, void* u2)
{
	if(this->pendingNavigation)
	{
		if(this->navigateForward)
		{
			// Navigate forward
			ControllerFactory* factory = GetControllerFactoryEntry(this->nextControllerName);
			if(factory == nullptr)
				return;

			if(this->noTrail)
			{
				this->chain->instance->Unload();
				app->Detach();
				PopController();
			}


			// Instantiate controller
			ControllerBase* newInstance = factory->factoryMethod(this);

			// Perform OnLeave on previous controller
			if(this->chain != nullptr)
				this->chain->instance->Leave();

			PushController(newInstance);
			app->Attach(newInstance->View);

			// Instantiated State, move to Running state
			newInstance->Load(this->data);
			NavigationProcessed(this, newInstance);
		}
		else
		{
			// Navigate back
			ObjectClass* returnedData = this->chain->instance->Unload();
			app->Detach();
			bool popResult = PopController();

			if (popResult)
			{
				this->chain->instance->Reload(returnedData);
				NavigationProcessed(this, this->chain->instance);
			}

			// If pop result is false, it is the end of the controller
			// It should end the application
			// TODO: Add this as event instead
			if(!popResult)
				ui_app_exit();
		}

		this->pendingNavigation = false;
	}
}

LIBAPI ControllerFactory::ControllerFactory(CString controllerName, ControllerFactoryMethod factory) :
	controllerName(controllerName), factoryMethod(factory)
{
	attachedData = nullptr;
}

LIBAPI ControllerBase* SRIN::Framework::StackingControllerManager::GetCurrentController() {
	return this->chain->instance;
}

