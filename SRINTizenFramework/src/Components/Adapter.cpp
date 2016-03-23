/*
 * Adapter.cpp
 *
 *  Created on: Mar 21, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Components/Adapter.h"

/* =================================================================================================================
 * IMPLEMENTATION: AdapterItemClassBase
 * ================================================================================================================= */

LIBAPI SRIN::Components::AdapterItemClassBase::~AdapterItemClassBase()
{
}


/* =================================================================================================================
 * IMPLEMENTATION: BasicAdapter
 * ================================================================================================================= */

LIBAPI SRIN::Components::Adapter::Adapter()
{
}

LIBAPI SRIN::Components::Adapter::~Adapter()
{
	Clear();
}

LIBAPI void SRIN::Components::Adapter::AddItemInternal(void* data, AdapterItemClassBase* itemClass)
{
	adapterItems.push_back({data, itemClass, nullptr});
	auto& lastItem = adapterItems.back();
	OnItemAdd(this, &lastItem);
}

LIBAPI void SRIN::Components::Adapter::RemoveItemInternal(void* data)
{
	for(auto iter = adapterItems.begin(); iter != adapterItems.end();)
	{
		if(iter->data == data)
		{
			auto& ref = *iter;
			OnItemRemove(this, &ref);
			iter = adapterItems.erase(iter);
		}
	}
}

LIBAPI std::list<SRIN::Components::Adapter::AdapterItem>& SRIN::Components::Adapter::GetAll()
{
	return adapterItems;
}

LIBAPI void SRIN::Components::Adapter::Clear(bool erase)
{
	for(auto iter = adapterItems.begin(); iter != adapterItems.end();)
	{
		auto& ref = *iter;

		// Notify the user of this adapter
		OnItemRemove(this, &ref);

		// Erase the object
		if(deallocate)
			ref.itemClass->Deallocator(ref.data);

		// Erase the entry
		iter = adapterItems.erase(iter);
	}
}

LIBAPI int SRIN::Components::Adapter::GetCount()
{
	return adapterItems.size();
}

/* =================================================================================================================
 * IMPLEMENTATION: BasicListAdapter
 * ================================================================================================================= */

SRIN::Components::BasicListAdapter::BasicListItemClass SRIN::Components::BasicListAdapter::basicListItemClass;

SRIN::Components::BasicListAdapter::BasicListAdapter()
{
}

SRIN::Components::BasicListAdapter::BasicListAdapter(std::initializer_list<std::string> init)
{
	for(auto& item : init)
	{
		AddItem(item);
	}
}

SRIN::Components::BasicListAdapter::~BasicListAdapter()
{

}

void SRIN::Components::BasicListAdapter::AddItem(std::string string)
{
	int currentCount = this->GetCount();
	Adapter::AddItem(new BasicAdapterItem({ currentCount, string }), &basicListItemClass);
}

void SRIN::Components::BasicListAdapter::RemoveItem(std::string string)
{
	// Unimplemented
}

std::string SRIN::Components::BasicListAdapter::BasicListItemClass::GetString(BasicAdapterItem* data, Evas_Object* obj,
	const char* part)
{
	return data->value;
}

Evas_Object* SRIN::Components::BasicListAdapter::BasicListItemClass::GetContent(BasicAdapterItem* data, Evas_Object* obj,
	const char* part)
{
	return nullptr;
}
