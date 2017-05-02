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
 *    Framework/ViewBase.cpp
 *
 * Created on:  Feb 15, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include <dlog.h>
#include "TFC/Framework/Application.h"

using namespace TFC::Framework;

LIBAPI ViewBase::ViewBase()
{
	viewRoot = nullptr;
}
LIBAPI Evas_Object* ViewBase::Create(Evas_Object* root)
{
	if (!this->viewRoot)
		this->viewRoot = CreateView(root);

	return viewRoot;
}

LIBAPI bool ViewBase::IsCreated()
{
	return viewRoot != nullptr;
}

LIBAPI Evas_Object* ViewBase::GetViewRoot()
{
	return viewRoot;
}

LIBAPI ViewBase::~ViewBase()
{

}

void TFC::Framework::ITitleProvider::SetTitle(const std::string& value)
{
	this->viewTitle = value;
	dlog_print(DLOG_DEBUG, LOG_TAG, "Set title on ViewBase %s", value.c_str());
}

std::string const& TFC::Framework::ITitleProvider::GetTitle() const
{
	return this->viewTitle;
}
