/*
 * GEnericList.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Components/GenericList.h"
#include <cstdlib>

using namespace SRIN::Components;

typedef struct
{
	GenericListItemClassBase* base;
	GenericList* genlist;
	void* data;
} GenlistItemClassPackage;

void Genlist_ClickedEventHandler(void* data, Evas_Object* obj, void* eventData)
{
	auto package = static_cast<GenlistItemClassPackage*>(data);
	package->genlist->OnItemClicked(package->genlist, package->data);
}

/* =================================================================================================================
 * IMPLEMENTATION: GenericListItemClassBase
 * ================================================================================================================= */

LIBAPI SRIN::Components::GenericListItemClassBase::GenericListItemClassBase(CString styleName)
{
	itemClass = elm_genlist_item_class_new();
	itemClass->item_style = styleName;

	// Callback redirect for get text function
	itemClass->func.text_get = [] (void *data, Evas_Object *obj, const char *part) ->  char*
	{
		auto pkg = reinterpret_cast<GenlistItemClassPackage*>(data);
		std::string ret = pkg->base->GetString(pkg->data, obj, part);
		return strdup(ret.c_str());
	};

	// Callback redirect for get content function
	itemClass->func.content_get = [] (void *data, Evas_Object *obj, const char *part) -> Evas_Object*
	{
		auto pkg = reinterpret_cast<GenlistItemClassPackage*>(data);
		return pkg->base->GetContent(pkg->data, obj, part);
	};

	// Callback redirect for delete function
	itemClass->func.del = [] (void *data, Evas_Object *obj)
	{
		auto pkg = reinterpret_cast<GenlistItemClassPackage*>(data);
		delete pkg;
	};
}

LIBAPI void* SRIN::Components::GenericListItemClassBase::operator ()(GenericList* genlist, void* itemData)
{
	return new GenlistItemClassPackage({this, genlist, itemData}) ;
}

LIBAPI SRIN::Components::GenericListItemClassBase::operator Elm_Genlist_Item_Class*()
{
	return this->itemClass;
}

LIBAPI SRIN::Components::GenericListItemClassBase::~GenericListItemClassBase()
{
	if(this->itemClass)
	{
		elm_genlist_item_class_free(this->itemClass);
		this->itemClass = nullptr;
	}
}

/* =================================================================================================================
 * IMPLEMENTATION: GenericListAdapter
 * ================================================================================================================= */

LIBAPI SRIN::Components::GenericListAdapter::GenericListAdapter()
{

}

LIBAPI SRIN::Components::GenericListAdapter::~GenericListAdapter()
{
	Clear();
}

LIBAPI void SRIN::Components::GenericListAdapter::AddItemInternal(void* data, GenericListItemClassBase* itemClass)
{
	adapterItems.push_back({data, itemClass, nullptr});
	auto& lastItem = adapterItems.back();
	OnItemAdd(this, &lastItem);
}

LIBAPI void SRIN::Components::GenericListAdapter::RemoveItemInternal(void* data)
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

LIBAPI std::list<GenericListAdapter::GenlistItem>& SRIN::Components::GenericListAdapter::GetAll()
{
	return adapterItems;
}

void SRIN::Components::GenericListAdapter::Clear(bool preserve)
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

int SRIN::Components::GenericListAdapter::GetCount()
{
	return adapterItems.size();
}

/* =================================================================================================================
 * IMPLEMENTATION: GenericList
 * ================================================================================================================= */

LIBAPI SRIN::Components::GenericList::GenericList() :
	adapter(nullptr), genlist(nullptr), Adapter(this), Overscroll(this), realBottom(nullptr)
{
	onScrolledBottomInternal += { this, &GenericList::OnScrolledBottomInternal };
	onScrolledTopInternal += { this, &GenericList::OnScrolledTopInternal };
	onDummyRealized += { this, &GenericList::OnDummyRealized };

	dummyBottom = nullptr;
	dummyBottomItemClass = elm_genlist_item_class_new();
	dummyBottomItemClass->item_style = "full";
	dummyBottomItemClass->func.content_get = [] (void *data, Evas_Object *obj, const char *part) -> Evas_Object*
	{
		auto dummyBox = elm_bg_add(obj);
		evas_object_color_set(dummyBox, 0, 0, 0, 0);
		evas_object_size_hint_weight_set(dummyBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(dummyBox, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_min_set(dummyBox, 0, 50);
		return dummyBox;
	};
}

LIBAPI void SRIN::Components::GenericList::SetAdapter(GenericListAdapter* const & newAdapter)
{
	// Unbind the old adapter
	if(adapter != nullptr)
	{
		auto oldList = adapter->GetAll();
		for(auto& item : oldList)
		{
			// Remove all item
			elm_object_item_del(item.objectItem);
			item.objectItem = nullptr;
		}

		// Remove event
		adapter->OnItemAdd -= { this, &GenericList::OnItemAdd };
		adapter->OnItemRemove -= { this, &GenericList::OnItemRemove };
	}

	// Assign new adapter
	this->adapter = newAdapter;

	// Append all existing item in adapter
	auto all = adapter->GetAll();
	for(auto& item : all)
	{
		AppendItemToGenlist(&item);
	}

	// Attach to event
	adapter->OnItemAdd += { this, &GenericList::OnItemAdd };
	adapter->OnItemRemove += { this, &GenericList::OnItemRemove };
}

LIBAPI void SRIN::Components::GenericList::AppendItemToGenlist(GenericListAdapter::GenlistItem* data)
{
	auto package = (*(data->itemClass))(this, data->data);
	if(realBottom != nullptr)
	{
		// BUG ON EFL: insert before dummy bottom will create UI render error if at some point dummy bottom is disabled
		// so the workaround is to store the realbottom of the list, which is an item before dummy bottom, and insert
		// after that item
		realBottom = elm_genlist_item_insert_after(genlist, *(data->itemClass), (*(data->itemClass))(this, data->data), nullptr, realBottom, ELM_GENLIST_ITEM_NONE, Genlist_ClickedEventHandler, package);
		data->objectItem = realBottom;
	}
	else
	{
		realBottom = elm_genlist_item_append(genlist, *(data->itemClass), package, nullptr, ELM_GENLIST_ITEM_NONE, Genlist_ClickedEventHandler, package);
		data->objectItem = realBottom;


		if(overscroll && !dummyBottom) // Create dummy item if overscroll enabled
			dummyBottom = elm_genlist_item_append(genlist, dummyBottomItemClass, dummyBottomItemClass, nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
	}
}

LIBAPI void SRIN::Components::GenericList::OnItemAdd(Event<GenericListAdapter*, GenericListAdapter::GenlistItem*>* event, GenericListAdapter* adapter,
	GenericListAdapter::GenlistItem* data)
{
	AppendItemToGenlist(data);
}

LIBAPI void SRIN::Components::GenericList::OnItemRemove(Event<GenericListAdapter*, GenericListAdapter::GenlistItem*>* event, GenericListAdapter* adapter,
	GenericListAdapter::GenlistItem* data)
{
	// BUG ON EFL: insert before dummy bottom will create UI render error if at some point dummy bottom is disabled
	// so the workaround is to store the realbottom of the list. Tis part is replacing the realBottom if the item
	// to be deleted is the real bottom
	if(data->objectItem == realBottom)
		realBottom = elm_genlist_item_prev_get(realBottom);

	elm_object_item_del(data->objectItem);
	data->objectItem = nullptr;

	// If it is the final object on the adapter
	if(adapter->GetCount() == 1 && dummyBottom)
	{
		elm_object_item_del(dummyBottom);
		dummyBottom = nullptr;
	}
}

LIBAPI Evas_Object* SRIN::Components::GenericList::CreateComponent(Evas_Object* root)
{
	genlist = elm_genlist_add(root);
	evas_object_smart_callback_add(genlist, "edge,top", SmartEventHandler, &onScrolledTopInternal);
	evas_object_smart_callback_add(genlist, "edge,bottom", SmartEventHandler, &onScrolledBottomInternal);
	evas_object_smart_callback_add(genlist, "realized", SmartEventHandler, &onDummyRealized);
	elm_genlist_highlight_mode_set(genlist, EINA_FALSE);
	return genlist;
}

LIBAPI GenericListAdapter*& SRIN::Components::GenericList::GetAdapter()
{
	return adapter;
}

void SRIN::Components::GenericList::OnScrolledBottomInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData)
{
	OnScrolledBottom(this, eventData);
}

void SRIN::Components::GenericList::OnScrolledTopInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData)
{
	OnScrolledTop(this, eventData);
}

void SRIN::Components::GenericList::OnDummyRealized(ElementaryEvent* event, Evas_Object* obj, void* eventData)
{
	if(eventData == dummyBottom)
	{
		// If the dummy bottom is realized, call the reaching bottom function
		OnReachingBottom(this, nullptr);
	}
}

void SRIN::Components::GenericList::SetOverscroll(const bool& o)
{
	this->overscroll = o;

	if(overscroll)
	{
		if(!dummyBottom)
			dummyBottom = elm_genlist_item_append(genlist, dummyBottomItemClass, dummyBottomItemClass, nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
	}
	else
	{
		if(dummyBottom)
		{
			elm_object_item_del(dummyBottom);
			dummyBottom = nullptr;
		}
	}
}

bool& SRIN::Components::GenericList::GetOverscroll()
{
	return this->overscroll;
}
