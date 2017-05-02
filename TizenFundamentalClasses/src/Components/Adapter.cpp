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
 *    Components/Adapter.cpp
 *
 * Created on:  Mar 21, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Ida Bagus Putu Peradnya Dinata (ib.putu@samsung.com)
 * 				Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/Adapter.h"

/* =================================================================================================================
 * IMPLEMENTATION: AdapterItemClassBase
 * ================================================================================================================= */

LIBAPI TFC::Components::AdapterItemClassBase::~AdapterItemClassBase()
{
}


/* =================================================================================================================
 * IMPLEMENTATION: BasicAdapter
 * ================================================================================================================= */

LIBAPI TFC::Components::Adapter::Adapter() :
	adapterItems()
{

}

LIBAPI TFC::Components::Adapter::~Adapter()
{
	Clear();
}

LIBAPI TFC::Components::Adapter::AdapterItem& TFC::Components::Adapter::AddItemInternal(void* data, AdapterItemClassBase* itemClass)
{
	adapterItems.push_back({ data, itemClass });
	auto& lastItem = adapterItems.back();
	eventItemAdd(this, { --(adapterItems.end()), adapterItems.begin(), adapterItems.end() });

	return lastItem;
}

LIBAPI void TFC::Components::Adapter::RemoveItemInternal(void* data)
{
	for(auto iter = adapterItems.begin(); iter != adapterItems.end(); )
	{
		if(iter->data == data)
		{
			auto& ref = *iter;
			eventItemRemove(this, &ref);
			iter = adapterItems.erase(iter);
		} else {
			iter++;
		}
	}
}

LIBAPI void TFC::Components::Adapter::RemoveItem(std::list<AdapterItem>::iterator iter)
{
	auto& ref = *iter;
	eventItemRemove(this, &ref);
	adapterItems.erase(iter);
}

LIBAPI std::list<TFC::Components::Adapter::AdapterItem>& TFC::Components::Adapter::GetAll()
{
	return adapterItems;
}

LIBAPI void TFC::Components::Adapter::Clear(bool deallocate)
{
	/*
	for(auto iter = adapterItems.begin(); iter != adapterItems.end();)
	{
		auto& ref = *iter;

		// Notify the user of this adapter
		eventItemRemove(this, &ref);

		// Erase the object
		if(deallocate)
			ref.itemClass->Deallocator(ref.data);

		// Erase the entry
		iter = adapterItems.erase(iter);
	}
	*/

	for(auto& item : adapterItems)
	{
		eventItemRemove(this, &item);

		if(deallocate)
			item.itemClass->Deallocator(item.data);
	}

	adapterItems.clear();
}

LIBAPI int TFC::Components::Adapter::GetCount()
{
	return adapterItems.empty() ? 0 : adapterItems.size();
}

/* =================================================================================================================
 * IMPLEMENTATION: BasicListAdapter
 * ================================================================================================================= */

TFC::Components::BasicListAdapter::BasicListItemClass TFC::Components::BasicListAdapter::basicListItemClass;

TFC::Components::BasicListAdapter::BasicListAdapter()
{
}

TFC::Components::BasicListAdapter::BasicListAdapter(std::initializer_list<std::string> init)
{
	for(auto& item : init)
	{
		AddItem(item);
	}
}

TFC::Components::BasicListAdapter::~BasicListAdapter()
{

}

void TFC::Components::BasicListAdapter::AddItem(std::string string)
{
	int currentCount = this->GetCount();
	Adapter::AddItem(new BasicAdapterItem({ currentCount, string }), &basicListItemClass);
}

void TFC::Components::BasicListAdapter::RemoveItem(std::string string)
{
	// Unimplemented
}

std::string TFC::Components::BasicListAdapter::BasicListItemClass::GetString(BasicAdapterItem* data, Evas_Object* obj,
	const char* part)
{
	return data->value;
}

Evas_Object* TFC::Components::BasicListAdapter::BasicListItemClass::GetContent(BasicAdapterItem* data, Evas_Object* obj,
	const char* part)
{
	return nullptr;
}

TFC::Components::Adapter::Adapter(const Adapter& that)
{
	this->adapterItems = that.adapterItems;
	/*
	for(auto item : adapterItems)
	{
		item.objectItem = nullptr;
	}
	*/
}

TFC::Components::Adapter::AdapterItem& TFC::Components::Adapter::AddItemFrontInternal(
		void* data, AdapterItemClassBase* itemClass) {
	adapterItems.push_front({ data, itemClass });
	auto& frontItem = adapterItems.front();
	eventItemAdd(this, { adapterItems.begin(), adapterItems.begin(), adapterItems.end() });

	return frontItem;
}
