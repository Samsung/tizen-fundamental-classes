/*
 * Adapter.cpp
 *
 *  Created on: Mar 21, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        ib.putu (ib.putu@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
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
	eventItemAdd(this, &lastItem);

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
