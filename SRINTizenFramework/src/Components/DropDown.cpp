/*
 * DropDown.cpp
 *
 *  Created on: Mar 21, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Components/DropDown.h"

#include <efl_extension.h>

using namespace SRIN::Components;

typedef struct {
	DropDown* adapter;
	Adapter::AdapterItem* item;
} DropdownCallbackPackage;

void DropdownAdapter_ClickCallback(void* data, Evas_Object* obj, void* eventInfo)
{
	DropdownCallbackPackage* package = static_cast<DropdownCallbackPackage*>(data);
	package->adapter->ItemClick(package->item);
}

void DropdownAdapter_DeleteCallback(void* data, Evas_Object* obj, void* eventInfo)
{
	DropdownCallbackPackage* package = static_cast<DropdownCallbackPackage*>(data);
	delete package;
}

LIBAPI Evas_Object* SRIN::Components::DropDown::CreateComponent(Evas_Object* root)
{
	Evas_Object* dropDown = elm_button_add(root);
	elm_object_style_set(dropDown, "dropdown");

	if(text.length())
		elm_object_text_set(dropDown, text.c_str());

	evas_object_smart_callback_add(dropDown, "clicked", SmartEventHandler, &dropdownButtonClick);
	this->parentComponent = root;
	return dropDown;
}

LIBAPI SRIN::Components::DropDown::DropDown() :
	dropdownComponent(nullptr),
	dataSource(nullptr),
	parentComponent(nullptr),
	selectedItem(nullptr),
	DataSource(this)
{
	dropdownButtonClick += { this, &DropDown::OnDropDownButtonClick };
}

LIBAPI void SRIN::Components::DropDown::ShowDropdown()
{
	dropdownComponent = elm_ctxpopup_add(this->parentComponent);
	elm_object_style_set(dropdownComponent, "dropdown/list");

	// Generate drop down list
	if(this->dataSource)
	{
		for(auto& item : this->dataSource->GetAll())
		{
			auto text = item.itemClass->GetString(item.data, dropdownComponent, "text");
			auto icon = item.itemClass->GetContent(item.data, dropdownComponent, "icon");
			auto package = new DropdownCallbackPackage({ this, &item });
			item.objectItem = elm_ctxpopup_item_append(dropdownComponent, text.c_str(), icon, DropdownAdapter_ClickCallback, package);
		}
	}

	elm_ctxpopup_direction_priority_set(dropdownComponent,
			ELM_CTXPOPUP_DIRECTION_DOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN,
			ELM_CTXPOPUP_DIRECTION_UNKNOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN);

	//disable back pressed listener on app
	//so back pressed listener on component can run
	// Framework::AppInstance->EnableBackButtonCallback(false);
	eext_object_event_callback_add(dropdownComponent, EEXT_CALLBACK_BACK, SmartEventHandler, &dropdownDismiss);
	evas_object_smart_callback_add(dropdownComponent, "dismissed", SmartEventHandler, &dropdownDismiss);

	//change position of the popup base on button
	Evas_Coord x, y;
	evas_pointer_canvas_xy_get(evas_object_evas_get(componentRoot), &x, &y);
	evas_object_move(dropdownComponent, x, y);
	evas_object_show(dropdownComponent);
}

LIBAPI void SRIN::Components::DropDown::OnDropDownButtonClick(ElementaryEvent* viewSource, Evas_Object* objSource,
	void* eventData)
{
	ShowDropdown();
}

LIBAPI void SRIN::Components::DropDown::ItemClick(Adapter::AdapterItem* item)
{
	selectedItem = item->data;
	text = item->itemClass->GetString(item->data, nullptr, "text");
	elm_object_text_set(componentRoot, text.c_str());

	if(dropdownComponent)
	{
		evas_object_del(dropdownComponent);
		dropdownComponent = nullptr;
	}

	// Dereference the pointer to object item as now it is invalid
	for(auto& item : this->dataSource->GetAll())
	{
		item.objectItem = nullptr;
	}

	ItemSelected(this, selectedItem);
}

LIBAPI void SRIN::Components::DropDown::SetDataSource(Adapter* adapter)
{
	this->dataSource = adapter;
}

LIBAPI Adapter* SRIN::Components::DropDown::GetDataSource()
{
	return this->dataSource;
}
