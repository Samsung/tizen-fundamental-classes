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

	if (package->genlist->IsLongClicked) {
		package->genlist->IsLongClicked = false;
		//package->genlist->ItemLongClicked(package->genlist, package->data);
	} else {
		package->genlist->eventItemClicked(package->genlist, package->data);
	}
}

/* =================================================================================================================
 * IMPLEMENTATION: GenericListItemClassBase
 * ================================================================================================================= */

LIBAPI SRIN::Components::GenericListItemClassBase::GenericListItemClassBase(CString styleName, bool defaultEventClick)
{
	itemClass = elm_genlist_item_class_new();
	itemClass->item_style = styleName;

	this->itemClickEnabled = defaultEventClick;

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
 * IMPLEMENTATION: GenericList
 * ================================================================================================================= */

LIBAPI SRIN::Components::GenericList::GenericList() :
	dataSource(nullptr), genlist(nullptr), realBottom(nullptr), DataSource(this), Overscroll(this), overscroll(false), IsLongClicked(this), BackToTopThreshold(this)
{
	eventScrolledInternal += EventHandler(GenericList::OnScrolledInternal);
	eventScrolledDownInternal += EventHandler(GenericList::OnScrolledDownInternal);
	eventScrolledUpInternal += EventHandler(GenericList::OnScrolledUpInternal);
	eventScrolledBottomInternal += EventHandler(GenericList::OnScrolledBottomInternal);
	eventScrolledTopInternal += EventHandler(GenericList::OnScrolledTopInternal);
	eventLongPressedInternal += EventHandler(GenericList::OnLongPressedInternal);
	eventDummyRealized += EventHandler(GenericList::OnDummyRealized);
	eventScrollingStartInternal += EventHandler(GenericList::OnScrollingStart);
	eventScrollingEndInternal += EventHandler(GenericList::OnScrollingEnd);
	eventItemSignalInternal += EventHandler(GenericList::OnItemSignalEmit);

	isScrolling = false;
	longpressed = false;
	backToTopThreshold = -1;
	backToTopShown = false;

	dummyBottom = nullptr;
	dummyBottomItemClass = elm_genlist_item_class_new();
	dummyBottomItemClass->item_style = "full";
	dummyBottomItemClass->func.content_get = [] (void *data, Evas_Object *obj, const char *part) -> Evas_Object*
	{
		GenericList* genlist = (GenericList*)data;
		Evas_Object* content = nullptr;

		genlist->eventDummyBottomContent(genlist, &content);

		if(content != nullptr)
			return content;

		auto dummyBox = elm_bg_add(obj);
		evas_object_color_set(dummyBox, 0, 0, 0, 0);
		evas_object_size_hint_weight_set(dummyBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(dummyBox, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_min_set(dummyBox, 0, 50);


		return dummyBox;
	};
}

LIBAPI void SRIN::Components::GenericList::ResetScroll(bool animated)
{
	if (animated) {
		elm_scroller_page_bring_in(genlist, 0, 0);
	}
	else {
		elm_scroller_page_show(genlist, 0, 0);
	}
}

LIBAPI void SRIN::Components::GenericList::SetDataSource(Adapter* newAdapter)
{
	// Unbind the old adapter
	if(dataSource != nullptr)
	{
		auto oldList = dataSource->GetAll();
		for(auto& item : oldList)
		{
			// Remove all item
			elm_object_item_del(item.objectItem);
			item.objectItem = nullptr;
		}

		// Remove event
		dataSource->eventItemAdd -= EventHandler(GenericList::OnItemAdd);
		dataSource->eventItemRemove -= EventHandler(GenericList::OnItemRemove);
	}

	// Assign new adapter
	this->dataSource = newAdapter;

	// Append all existing item in adapter
	auto all = dataSource->GetAll();
	for(auto& item : all)
	{
		AppendItemToGenlist(&item);
	}
	// Attach to event
	dataSource->eventItemAdd += EventHandler(GenericList::OnItemAdd);
	dataSource->eventItemRemove += EventHandler(GenericList::OnItemRemove);
}

LIBAPI void SRIN::Components::GenericList::AppendItemToGenlist(Adapter::AdapterItem* data)
{
	auto itemClass = dynamic_cast<GenericListItemClassBase*>(data->itemClass);
	if(itemClass == nullptr)
		throw std::runtime_error("Invalid item class specified for generic list. Use GenericListItemClass type");

	auto package = (*(itemClass))(this, data->data);

	decltype(Genlist_ClickedEventHandler)* handlerPtr = nullptr;
	void* eventPackage = nullptr;

	if(itemClass->IsItemClickEnabled())
	{
		handlerPtr = &Genlist_ClickedEventHandler;
		eventPackage = package;
	}

	if(realBottom != nullptr)
	{
		// BUG ON EFL: insert before dummy bottom will create UI render error if at some point dummy bottom is disabled
		// so the workaround is to store the realbottom of the list, which is an item before dummy bottom, and insert
		// after that item
		realBottom = elm_genlist_item_insert_after(genlist, *(itemClass), (*(itemClass))(this, data->data), nullptr, realBottom, ELM_GENLIST_ITEM_NONE, handlerPtr, eventPackage);
		data->objectItem = realBottom;
	}
	else
	{
		realBottom = elm_genlist_item_append(genlist, *(itemClass), package, nullptr, ELM_GENLIST_ITEM_NONE, handlerPtr, eventPackage);
		data->objectItem = realBottom;


		if(overscroll && !dummyBottom) // Create dummy item if overscroll enabled
			dummyBottom = elm_genlist_item_append(genlist, dummyBottomItemClass, this, nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
	}


	elm_object_item_signal_callback_add(realBottom, "*", "*", [] (void *data, Evas_Object *obj, const char *emission, const char *source) {
			dlog_print(DLOG_DEBUG, "SRINFW-Signal", "Signal SRIN %s, source %s", emission, source);
	}, nullptr);
}

LIBAPI void SRIN::Components::GenericList::OnItemAdd(Event<Adapter*, Adapter::AdapterItem*>* event, Adapter* adapter,
	Adapter::AdapterItem* data)
{
	AppendItemToGenlist(data);
}

LIBAPI void SRIN::Components::GenericList::OnItemRemove(Event<Adapter*, Adapter::AdapterItem*>* event, Adapter* adapter,
	Adapter::AdapterItem* data)
{
	// BUG ON EFL: insert before dummy bottom will create UI render error if at some point dummy bottom is disabled
	// so the workaround is to store the realbottom of the list. Tis part is replacing the realBottom if the item
	// to be deleted is the real bottom
	if(data->objectItem == realBottom)
		realBottom = elm_genlist_item_prev_get(realBottom);

	elm_object_item_del(data->objectItem);
	data->objectItem = nullptr;

	// If this is last, then just remove the dummy bottom
	if(adapter->GetCount() == 1)
	{
		realBottom = nullptr;

		if(dummyBottom)
		{
			elm_object_item_del(dummyBottom);
			dummyBottom = nullptr;
		}

	}
}

LIBAPI Evas_Object* SRIN::Components::GenericList::CreateComponent(Evas_Object* root)
{
	genlist = elm_genlist_add(root);
	evas_object_smart_callback_add(genlist, "scroll", EFL::EvasSmartEventHandler, &eventScrolledInternal);
	evas_object_smart_callback_add(genlist, "scroll,down", EFL::EvasSmartEventHandler, &eventScrolledDownInternal);
	evas_object_smart_callback_add(genlist, "scroll,up", EFL::EvasSmartEventHandler, &eventScrolledUpInternal);
	evas_object_smart_callback_add(genlist, "edge,top", EFL::EvasSmartEventHandler, &eventScrolledTopInternal);
	evas_object_smart_callback_add(genlist, "edge,bottom", EFL::EvasSmartEventHandler, &eventScrolledBottomInternal);
	evas_object_smart_callback_add(genlist, "realized", EFL::EvasSmartEventHandler, &eventDummyRealized);
	evas_object_smart_callback_add(genlist, "scroll,drag,start", EFL::EvasSmartEventHandler, &eventScrollingStartInternal);
	evas_object_smart_callback_add(genlist, "scroll,drag,stop", EFL::EvasSmartEventHandler, &eventScrollingEndInternal);
	evas_object_smart_callback_add(genlist, "longpressed", EFL::EvasSmartEventHandler, &eventLongPressedInternal);
	elm_genlist_highlight_mode_set(genlist, EINA_FALSE);
	elm_genlist_select_mode_set(genlist, ELM_OBJECT_SELECT_MODE_ALWAYS);

	return genlist;
}

LIBAPI SRIN::Components::Adapter* SRIN::Components::GenericList::GetDataSource()
{
	return dataSource;
}

void SRIN::Components::GenericList::OnScrolledInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData)
{
	int x, y, w, h;
	elm_scroller_region_get(genlist, &x, &y, &w, &h);
	if (backToTopThreshold < 0) {
		evas_object_geometry_get(genlist, nullptr, nullptr, nullptr, &backToTopThreshold);
	}

	bool show = (y > backToTopThreshold);
	if (backToTopShown != show) {
		backToTopShown = show;
		eventShowBackToTop(this, &backToTopShown);
	}
	eventScrolled(this, eventData);
}

void SRIN::Components::GenericList::OnScrolledDownInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData)
{
	eventScrolledDown(this, eventData);
}

void SRIN::Components::GenericList::OnScrolledUpInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData)
{
	eventScrolledUp(this, eventData);
}

void SRIN::Components::GenericList::OnScrolledBottomInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData)
{
	eventScrolledBottom(this, eventData);
}

void SRIN::Components::GenericList::OnScrolledTopInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData)
{
	eventScrolledTop(this, eventData);
}

void SRIN::Components::GenericList::OnDummyRealized(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData)
{
	if(eventData == dummyBottom)
	{
		// If the dummy bottom is realized, call the reaching bottom function
		eventReachingBottom(this, nullptr);
	}
	else
	{
		auto ret = elm_object_item_widget_get((Elm_Object_Item*)eventData);

		dlog_print(DLOG_DEBUG, LOG_TAG, "Widget Realized SRIN %d", obj);
	}
}

void SRIN::Components::GenericList::SetOverscroll(const bool& o)
{
	this->overscroll = o;

	if(overscroll)
	{
		if(!dummyBottom && dataSource && dataSource->GetCount() != 0)
			dummyBottom = elm_genlist_item_append(genlist, dummyBottomItemClass, this, nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
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

bool SRIN::Components::GenericList::GetOverscroll()
{
	return this->overscroll;
}

void SRIN::Components::GenericList::OnScrollingStart(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData)
{
	isScrolling   = true;
	IsLongClicked = false;
	eventScrollingStart(this, nullptr);
}

void SRIN::Components::GenericList::OnScrollingEnd(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData)
{
	isScrolling = false;
	eventScrollingEnd(this, nullptr);
}

void SRIN::Components::GenericList::OnItemSignalEmit(EFL::ObjectItemEdjeSignalEvent* event, Elm_Object_Item* obj,
	EFL::EdjeSignalInfo eventData)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Signal %s", eventData.source);

	auto data = reinterpret_cast<Adapter::AdapterItem*>(elm_object_item_data_get(obj));
	eventItemSignal(data, eventData);
}

void SRIN::Components::GenericList::SetLongClicked(const bool& o) {
	longpressed = o;
}

bool SRIN::Components::GenericList::GetLongClicked() {
	return longpressed;
}

void SRIN::Components::GenericList::OnLongPressedInternal(
		EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData) {
	if (isScrolling) {
		IsLongClicked = false;
	} else {
		IsLongClicked = true;
		Elm_Object_Item* temp1 = (Elm_Object_Item*) eventData;
		auto data = reinterpret_cast<GenlistItemClassPackage*>(elm_object_item_data_get(temp1));

		eventItemLongClicked(data->genlist, data->data);
	}
}

bool SRIN::Components::GenericListItemClassBase::IsItemClickEnabled() {
	return this->itemClickEnabled;
}
