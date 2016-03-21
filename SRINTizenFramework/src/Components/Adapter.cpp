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

LIBAPI void SRIN::Components::Adapter::Clear(bool preserve)
{
	for(auto iter = adapterItems.begin(); iter != adapterItems.end();)
	{
		auto& ref = *iter;

		// Notify the user of this adapter
		OnItemRemove(this, &ref);

		// Erase the object
		if(!preserve)
			ref.itemClass->Deallocator(ref.data);

		// Erase the entry
		iter = adapterItems.erase(iter);
	}
}

LIBAPI int SRIN::Components::Adapter::GetCount()
{
	return adapterItems.size();
}
