/*
 * ControllerManager.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
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

	this->chain = nullptr;
}

LIBAPI void StackingControllerManager::NavigateTo(const char* controllerName, void* data)
{
	ControllerFactory* factory = GetControllerFactoryEntry(controllerName);
	if (factory != nullptr)
	{
		ControllerBase* newInstance = factory->factoryMethod(this);

		if(this->chain != nullptr)
			this->chain->instance->Leave();

		PushController(newInstance);
		app->Attach(newInstance->View);
		newInstance->Load(data);
		NavigationProcessed(this, newInstance);
	}
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
	void* returnedData = this->chain->instance->Unload();
	app->Detach();
	bool popResult = PopController();

	if (popResult)
	{
		this->chain->instance->Reload(returnedData);
		NavigationProcessed(this, this->chain->instance);
	}

	return popResult;
}

LIBAPI void StackingControllerManager::NavigateTo(const char* controllerName, void* data, bool noTrail)
{
	if (noTrail)
	{
		this->chain->instance->Unload();
		app->Detach();
		PopController();
	}

	NavigateTo(controllerName, data);
}

LIBAPI ControllerFactory::ControllerFactory(CString controllerName, ControllerFactoryMethod factory) :
	controllerName(controllerName), factoryMethod(factory)
{
	attachedData = nullptr;
}

LIBAPI ControllerBase* SRIN::Framework::StackingControllerManager::GetCurrentController() {
	return this->chain->instance;
}

