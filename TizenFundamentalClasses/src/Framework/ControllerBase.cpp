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
 *    Framework/ApplicationBase.cpp
 *
 * Created on:  Feb 15, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Framework/Application.h"

using namespace TFC;
using namespace TFC::Framework;

LIBAPI ControllerBase::ControllerBase(ControllerManager* m, ViewBase* v, char const* c) :
	Manager(m), View(v), ControllerName(c)
{

}

LIBAPI void ControllerBase::OnLoad(ObjectClass* data)
{
}

LIBAPI ObjectClass* ControllerBase::OnUnload()
{
	return NULL;
}

LIBAPI void ControllerBase::Load(ObjectClass* data)
{
	OnLoad(data);
}

LIBAPI ObjectClass* ControllerBase::Unload()
{
	return OnUnload();
}

LIBAPI void ControllerBase::OnReload(ObjectClass* data)
{
}

LIBAPI void ControllerBase::Reload(ObjectClass* data)
{
	OnReload(data);
}

LIBAPI ControllerBase::~ControllerBase()
{
}


void TFC::Framework::ControllerBase::OnLeaving() {
}

void TFC::Framework::ControllerBase::Leave() {
	this->OnLeaving();
}
