/*
 * BackButtonHandler.cpp
 *
 *  Created on: Apr 27, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/BackButtonHandler.h"

bool TFC::Components::BackButtonHandler::BackButtonClickedInternal()
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Back Button Clicked");
	this->Release();
	return this->BackButtonClicked();

}

void TFC::Components::BackButtonHandler::Acquire()
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Virtual address: %p", &BackButtonHandler::BackButtonClicked);

	if(not this->acquired)
	{
		this->acquired = true;
		this->obj = new Wrapper(this);
		Framework::UIApplicationBase::CurrentInstance->AcquireExclusiveBackButtonPressed(this->obj, reinterpret_cast<Framework::BackButtonCallback>(&Wrapper::Call));
	}

}

void TFC::Components::BackButtonHandler::Release()
{
	if(this->acquired)
	{
		this->acquired = false;
		Framework::UIApplicationBase::CurrentInstance->ReleaseExclusiveBackButtonPressed(this->obj, reinterpret_cast<Framework::BackButtonCallback>(&Wrapper::Call));

		delete this->obj;
		this->obj = nullptr;
	}
}

TFC::Components::BackButtonHandler::BackButtonHandler() :
		obj(nullptr),
		acquired(false)
{
}

TFC::Components::BackButtonHandler::~BackButtonHandler()
{
	Release();
}

TFC::Components::BackButtonHandler::Wrapper::Wrapper(
		BackButtonHandler* handler) : handler(handler){
}

void TFC::Components::BackButtonHandler::Wrapper::Call() {
	handler->BackButtonClickedInternal();
}
