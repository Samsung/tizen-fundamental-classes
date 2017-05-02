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
 *    Components/ComponentBase.cpp
 *
 * Created on:  Feb 19, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/ComponentBase.h"

using namespace TFC::Components;

ComponentBase::ComponentBase() :
	Enabled(this), Visible(this), Name(this)
{
	weightX = EVAS_HINT_EXPAND;
	alignY = EVAS_HINT_FILL;
	componentRoot = nullptr;
	created = false;
	enabled = true;
	visible = true;
	name = "";

	alignX = EVAS_HINT_FILL;
	weightY = EVAS_HINT_EXPAND;
}

ComponentBase::~ComponentBase()
{

}

Evas_Object* ComponentBase::Create(Evas_Object* root)
{
	if (!created)
	{
		this->componentRoot = CreateComponent(root);
		this->created = true;

		evas_object_size_hint_weight_set(this->componentRoot, weightX, weightY);
		evas_object_size_hint_align_set(this->componentRoot, alignX, alignY);

		if (visible)
			evas_object_show(componentRoot);
		else
			evas_object_hide(componentRoot);

		if (!enabled)
			elm_object_disabled_set(this->componentRoot, EINA_TRUE);
	}

	return this->componentRoot;
}

ComponentBase::operator Evas_Object*() const
{
	return this->componentRoot;
}

void ComponentBase::SetEnabled(const bool& enabled)
{
	this->enabled = enabled;

	if (componentRoot)
		elm_object_disabled_set(componentRoot, !enabled);
}

bool ComponentBase::GetEnabled() const
{
	return this->enabled;
}

void ComponentBase::SetVisible(const bool& visible)
{
	this->visible = visible;

	if (componentRoot) {
		if (visible)
			evas_object_show(componentRoot);
		else
			evas_object_hide(componentRoot);
	}
}

bool ComponentBase::GetVisible() const
{
	return this->visible;
}

void ComponentBase::SetName(const std::string& name)
{
	this->name = name;
}

std::string const& ComponentBase::GetName() const
{
	return this->name;
}

