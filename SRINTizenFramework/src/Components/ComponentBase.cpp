/*
 * ComponentBase.cpp
 *
 *  Created on: Feb 19, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "SRIN/Components/ComponentBase.h"

using namespace SRIN::Components;

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

bool ComponentBase::GetEnabled()
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

bool ComponentBase::GetVisible()
{
	return this->visible;
}

void ComponentBase::SetName(const std::string& name)
{
	this->name = name;
}

std::string& ComponentBase::GetName()
{
	return this->name;
}

