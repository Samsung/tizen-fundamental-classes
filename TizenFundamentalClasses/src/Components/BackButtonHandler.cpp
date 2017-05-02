/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Components/BackButtonHandler.cpp
 *
 * Created on:  Apr 27, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/BackButtonHandler.h"
#include <dlog.h>

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
		auto uiApplicationBase = dynamic_cast<Framework::UIApplicationBase*>(Framework::UIApplicationBase::GetCurrentInstance());
		uiApplicationBase->AcquireExclusiveBackButtonPressed(this->obj, reinterpret_cast<Framework::BackButtonCallback>(&Wrapper::Call));
	}

}

void TFC::Components::BackButtonHandler::Release()
{
	if(this->acquired)
	{
		this->acquired = false;
		auto uiApplicationBase = dynamic_cast<Framework::UIApplicationBase*>(Framework::UIApplicationBase::GetCurrentInstance());
		uiApplicationBase->ReleaseExclusiveBackButtonPressed(this->obj, reinterpret_cast<Framework::BackButtonCallback>(&Wrapper::Call));

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
