/*
 * GEnericList.cpp
 *
 *  Created on: Mar 10, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        ib.putu (ib.putu@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/GenericList.h"
#include <dlog.h>

using namespace TFC::Components;

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

LIBAPI TFC::Components::GenericListItemClassBase::GenericListItemClassBase(char const* styleName, bool defaultEventClick)
{
	itemClass = elm_genlist_item_class_new();
	itemClass->item_style = styleName;

	this->itemClickEnabled = defaultEventClick;

	// Callback redirect for get text function
	itemClass->func.text_get = [] (void *data, Evas_Object *obj, const char *part) ->  char*
	{
		auto pkg = reinterpret_cast<GenlistItemClassPackage*>(data);
		std::string ret = pkg->base->GetString(pkg->data, obj, part);
		if(ret.empty()) return nullptr;
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

LIBAPI void* TFC::Components::GenericListItemClassBase::operator ()(GenericList* genlist, void* itemData)
{
	return new GenlistItemClassPackage({this, genlist, itemData}) ;
}

LIBAPI TFC::Components::GenericListItemClassBase::operator Elm_Genlist_Item_Class*()
{
	return this->itemClass;
}

LIBAPI TFC::Components::GenericListItemClassBase::~GenericListItemClassBase()
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

LIBAPI TFC::Components::GenericList::GenericList() :
	genlist(nullptr), realBottom(nullptr), dataSource(nullptr), overscroll(false), underscroll(false),
	DataSource(this), Overscroll(this), Underscroll(this),
	IsLongClicked(this), BackToTopThreshold(this)
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
	eventItemUnrealized += EventHandler(GenericList::OnItemUnrealized);

	isScrolling = false;
	longpressed = false;
	backToTopThreshold = -1;
	backToTopShown = false;
	firstRealize = true;

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

	dummyTop = nullptr;
	dummyTopItemClass = elm_genlist_item_class_new();
	dummyTopItemClass->item_style = "full";
	dummyTopItemClass->func.content_get = [] (void *data, Evas_Object *obj, const char *part) -> Evas_Object*
	{
		GenericList* genlist = (GenericList*)data;
		Evas_Object* content = nullptr;

		genlist->eventDummyTopContent(genlist, &content);

		if(content != nullptr)
			return content;

		auto dummyBox = elm_bg_add(obj);
		evas_object_color_set(dummyBox, 0, 0, 0, 0);
		evas_object_size_hint_weight_set(dummyBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(dummyBox, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_min_set(dummyBox, 0, 150);


		return dummyBox;
	};
}

LIBAPI
TFC::Components::GenericList::~GenericList()
{
	/*
	evas_object_smart_callback_del(genlist, "scroll", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(genlist, "scroll,down", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(genlist, "scroll,up", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(genlist, "edge,top", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(genlist, "edge,bottom", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(genlist, "scroll,drag,start", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(genlist, "scroll,drag,stop", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(genlist, "realized", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(genlist, "unrealized", EFL::EvasSmartEventHandler);
	*/

	this->dataSource->eventItemAdd -= EventHandler(GenericList::OnItemAdd);
	this->dataSource->eventItemRemove -= EventHandler(GenericList::OnItemRemove);
}

LIBAPI void TFC::Components::GenericList::ResetScroll(bool animated)
{
	if (animated) {
		elm_scroller_page_bring_in(genlist, 0, 0);
	}
	else {
		elm_scroller_page_show(genlist, 0, 0);
	}
	if (underscroll) {
		if (dummyTop == nullptr)
			dummyTop = elm_genlist_item_prepend(genlist, dummyTopItemClass, this, nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
		elm_genlist_item_show(elm_genlist_item_next_get(dummyTop), ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}
}

LIBAPI
void TFC::Components::GenericList::SetDataSource(Adapter* newAdapter)
{
	// Unbind the old adapter
	if(dataSource != nullptr)
	{
		for(auto& item : this->itemIndex)
		{
			// Remove all item
			elm_object_item_del(item.second);
		}

		itemIndex.clear();

		// Remove event
		dataSource->eventItemAdd -= EventHandler(GenericList::OnItemAdd);
		dataSource->eventItemRemove -= EventHandler(GenericList::OnItemRemove);

	}

	// Assign new adapter
	this->dataSource = newAdapter;
	firstRealize = true;

	// Append all existing item in adapter
	auto& all = dataSource->GetAll();

	for(auto& item : all)
	{
		AppendItemToGenlist(&item);
	}

	// Attach to event
	dataSource->eventItemAdd += EventHandler(GenericList::OnItemAdd);
	dataSource->eventItemRemove += EventHandler(GenericList::OnItemRemove);
}

LIBAPI
void TFC::Components::GenericList::AppendItemToGenlist(Adapter::AdapterItem* data)
{
	auto itemClass = dynamic_cast<GenericListItemClassBase*>(data->itemClass);
	if(itemClass == nullptr)
		throw std::runtime_error("Invalid item class specified for generic list. Use GenericListItemClass type");

	auto package = (*(itemClass))(this, data->data);

	decltype(Genlist_ClickedEventHandler)* handlerPtr = nullptr;
	void* eventPackage = nullptr;

	Elm_Object_Item* inserted = nullptr;

	if(itemClass->IsItemClickEnabled())
	{
		handlerPtr = &Genlist_ClickedEventHandler;
		eventPackage = package;
	}

	if (elm_genlist_items_count(genlist) == 0)
	{
		if (underscroll)
			dummyTop = elm_genlist_item_prepend(genlist, dummyTopItemClass, this, nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
	}

	if(realBottom != nullptr)
	{
		// BUG ON EFL: insert before dummy bottom will create UI render error if at some point dummy bottom is disabled
		// so the workaround is to store the realbottom of the list, which is an item before dummy bottom, and insert
		// after that item
		inserted = realBottom = elm_genlist_item_insert_after(genlist, *(itemClass), (*(itemClass))(this, data->data), nullptr, realBottom, ELM_GENLIST_ITEM_NONE, handlerPtr, eventPackage);
		//data->objectItem = realBottom;
	}
	else
	{
		inserted = realBottom = elm_genlist_item_append(genlist, *(itemClass), package, nullptr, ELM_GENLIST_ITEM_NONE, handlerPtr, eventPackage);
		//data->objectItem = realBottom;


		if(overscroll && !dummyBottom) // Create dummy item if overscroll enabled
			dummyBottom = elm_genlist_item_append(genlist, dummyBottomItemClass, this, nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
	}


	this->itemIndex.insert({ data, inserted });

	/*
	elm_object_item_signal_callback_add(realBottom, "*", "*", [] (void *data, Evas_Object *obj, const char *emission, const char *source) {
			dlog_print(DLOG_DEBUG, "TFCFW-Signal", "Signal TFC %s, source %s", emission, source);
	}, nullptr);
	*/
}

LIBAPI
void TFC::Components::GenericList::PrependItemToGenlist(Adapter::AdapterItem* data)
{
	auto itemClass = dynamic_cast<GenericListItemClassBase*>(data->itemClass);
	if(itemClass == nullptr)
		throw std::runtime_error("Invalid item class specified for generic list. Use GenericListItemClass type");

	auto package = (*(itemClass))(this, data->data);

	decltype(Genlist_ClickedEventHandler)* handlerPtr = nullptr;
	void* eventPackage = nullptr;

	Elm_Object_Item* inserted = nullptr;

	if(itemClass->IsItemClickEnabled())
	{
		handlerPtr = &Genlist_ClickedEventHandler;
		eventPackage = package;
	}

	if (elm_genlist_items_count(genlist) == 0)
	{
		if (underscroll)
			dummyTop = elm_genlist_item_prepend(genlist, dummyTopItemClass, this, nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
	}


	if(!underscroll)
	{
		inserted = elm_genlist_item_prepend(genlist, *(itemClass), package, nullptr, ELM_GENLIST_ITEM_NONE, handlerPtr, eventPackage);
		//data->objectItem = realBottom;
	}
	else
	{
		inserted = elm_genlist_item_insert_after(genlist, *(itemClass), (*(itemClass))(this, data->data), nullptr, dummyTop, ELM_GENLIST_ITEM_NONE, handlerPtr, eventPackage);
	}



	this->itemIndex.insert({ data, inserted });

	/*
	elm_object_item_signal_callback_add(realBottom, "*", "*", [] (void *data, Evas_Object *obj, const char *emission, const char *source) {
			dlog_print(DLOG_DEBUG, "TFCFW-Signal", "Signal TFC %s, source %s", emission, source);
	}, nullptr);
	*/
}

LIBAPI void TFC::Components::GenericList::OnItemAdd(Adapter* adapter, Adapter::ItemAddEventArgs data)
{
	auto theItem = data.item;
	++theItem;
	if(theItem == data.back)
	{
		// If it is in behind
		auto& ref = *(data.item);
		AppendItemToGenlist(&ref);
	}
	else if(data.item == data.front)
	{
		// If it is in front
		auto& ref = *(data.item);
		PrependItemToGenlist(&ref);
	}
}

LIBAPI void TFC::Components::GenericList::OnItemRemove(Adapter* adapter, Adapter::AdapterItem* data)
{
	// BUG ON EFL: insert before dummy bottom will create UI render error if at some point dummy bottom is disabled
	// so the workaround is to store the realbottom of the list. Tis part is replacing the realBottom if the item
	// to be deleted is the real bottom
	Elm_Object_Item* genlistItem = nullptr;

	try
	{
		genlistItem = this->itemIndex.at(data);
	}
	catch(std::out_of_range const& ex)
	{
		throw TFC::TFCException("Invalid removal data");
	}

	if(genlistItem == realBottom)
		realBottom = elm_genlist_item_prev_get(realBottom);

	elm_object_item_del(genlistItem);
	//data->objectItem = nullptr;

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

LIBAPI Evas_Object* TFC::Components::GenericList::CreateComponent(Evas_Object* root)
{
	genlist = elm_genlist_add(root);


	/*
	evas_object_smart_callback_add(genlist, "scroll", EFL::EvasSmartEventHandler, &eventScrolledInternal);
	evas_object_smart_callback_add(genlist, "scroll,down", EFL::EvasSmartEventHandler, &eventScrolledDownInternal);
	evas_object_smart_callback_add(genlist, "scroll,up", EFL::EvasSmartEventHandler, &eventScrolledUpInternal);
	evas_object_smart_callback_add(genlist, "edge,top", EFL::EvasSmartEventHandler, &eventScrolledTopInternal);
	evas_object_smart_callback_add(genlist, "edge,bottom", EFL::EvasSmartEventHandler, &eventScrolledBottomInternal);
	evas_object_smart_callback_add(genlist, "realized", EFL::EvasSmartEventHandler, &eventDummyRealized);
	evas_object_smart_callback_add(genlist, "scroll,drag,start", EFL::EvasSmartEventHandler, &eventScrollingStartInternal);
	evas_object_smart_callback_add(genlist, "scroll,drag,stop", EFL::EvasSmartEventHandler, &eventScrollingEndInternal);
	evas_object_smart_callback_add(genlist, "longpressed", EFL::EvasSmartEventHandler, &eventLongPressedInternal);
	evas_object_smart_callback_add(genlist, "unrealized", EFL::EvasSmartEventHandler, &eventItemUnrealized);
	*/

	eventScrolledInternal.Bind(genlist, "scroll");
	eventScrolledDownInternal.Bind(genlist, "scroll,down");
	eventScrolledUpInternal.Bind(genlist, "scroll,up");
	eventScrolledTopInternal.Bind(genlist, "edge,top");
	eventScrolledBottomInternal.Bind(genlist, "edge,bottom");
	eventDummyRealized.Bind(genlist, "realized");
	eventScrollingStartInternal.Bind(genlist, "scroll,drag,start");
	eventScrollingEndInternal.Bind(genlist, "scroll,drag,stop");
	eventLongPressedInternal.Bind(genlist, "longpressed");
	eventItemUnrealized.Bind(genlist, "unrealized");
	
	elm_genlist_highlight_mode_set(genlist, EINA_FALSE);
	elm_genlist_select_mode_set(genlist, ELM_OBJECT_SELECT_MODE_ALWAYS);

	return genlist;
}

LIBAPI TFC::Components::Adapter* TFC::Components::GenericList::GetDataSource()
{
	return dataSource;
}

void TFC::Components::GenericList::OnScrolledInternal(Evas_Object* obj, void* eventData)
{
	int x, y, w, h;
	elm_scroller_region_get(genlist, &x, &y, &w, &h);

	if(currentPosY>y){
		eventScrolledUp(this, eventData);
	}else{
		eventScrolledDown(this, eventData);
	}

	currentPosY = y;

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

void TFC::Components::GenericList::OnScrolledDownInternal(Evas_Object* obj, void* eventData)
{
	eventScrolledDown(this, eventData);
}

void TFC::Components::GenericList::OnScrolledUpInternal(Evas_Object* obj, void* eventData)
{
	eventScrolledUp(this, eventData);
}

void TFC::Components::GenericList::OnScrolledBottomInternal(Evas_Object* obj, void* eventData)
{
	eventScrolledBottom(this, eventData);
}

void TFC::Components::GenericList::OnScrolledTopInternal(Evas_Object* obj, void* eventData)
{
	eventScrolledTop(this, eventData);
}

void TFC::Components::GenericList::OnDummyRealized(Evas_Object* obj, void* eventData)
{
	if (firstRealize)
	{
		firstRealize = false;
		if (underscroll)
			elm_genlist_item_show(elm_genlist_item_next_get(dummyTop), ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}

	if(eventData == dummyBottom)
	{
		// If the dummy bottom is realized, call the reaching bottom function
		eventReachingBottom(this, nullptr);
	}
	else if (dummyTop == nullptr && eventData == elm_genlist_first_item_get(genlist))
	{
		eventReachingTop(this, nullptr);
		ecore_timer_add(1.0, [] (void* data) -> Eina_Bool {
			GenericList* gl = (GenericList*)data;
			dlog_print(DLOG_VERBOSE, LOG_TAG, "Prepend dummy top");
			if (gl->underscroll)
				gl->dummyTop = elm_genlist_item_prepend(gl->genlist, gl->dummyTopItemClass, gl, nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
			return ECORE_CALLBACK_CANCEL;
		}, this);
	}
	else if (eventData == dummyTop)
	{
		dlog_print(DLOG_VERBOSE, LOG_TAG, "Dummy top realize");
		elm_genlist_item_show(elm_genlist_item_next_get(dummyTop), ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}
	else if (dummyTop != nullptr && eventData == elm_genlist_item_next_get(dummyTop))
	{
		elm_genlist_item_show(elm_genlist_item_next_get(dummyTop), ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}
	else
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "Widget Realized TFC %d", obj);
	}
}

void TFC::Components::GenericList::OnItemUnrealized(Evas_Object* obj, void* eventData)
{
	if(dummyTop && eventData == elm_genlist_item_next_get(dummyTop))
	{
		dlog_print(DLOG_VERBOSE, LOG_TAG, "Delete dummy top");
		elm_object_item_del(dummyTop);
		dummyTop = nullptr;
	}
}

void TFC::Components::GenericList::SetOverscroll(const bool& o)
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

bool TFC::Components::GenericList::GetOverscroll() const
{
	return this->overscroll;
}

void TFC::Components::GenericList::SetUnderscroll(const bool& o)
{
	this->underscroll = o;
}

bool TFC::Components::GenericList::GetUnderscroll() const
{
	return this->underscroll;
}


void TFC::Components::GenericList::OnScrollingStart(Evas_Object* obj, void* eventData)
{
	isScrolling   = true;
	IsLongClicked = false;
	eventScrollingStart(this, nullptr);
}

void TFC::Components::GenericList::OnScrollingEnd(Evas_Object* obj, void* eventData)
{
	if (underscroll && dummyTop)
	{
		int y;
		elm_scroller_region_get(genlist, nullptr, &y, nullptr, nullptr);
		if (y == 0) {
			elm_object_item_del(dummyTop);
			dummyTop = nullptr;
			eventUnderscrolled(this, nullptr);
		}
		else {
			int x, y;
			evas_object_geometry_get(genlist, &x, &y, nullptr, nullptr);
			if (elm_genlist_at_xy_item_get(genlist, x, y, nullptr) == dummyTop) {
				elm_genlist_item_show(elm_genlist_item_next_get(dummyTop), ELM_GENLIST_ITEM_SCROLLTO_TOP);
			}
		}
	}
	isScrolling = false;
	eventScrollingEnd(this, nullptr);
}

void TFC::Components::GenericList::OnItemSignalEmit(Elm_Object_Item* obj, EFL::EdjeSignalInfo eventData)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Signal %s", eventData.source);

	auto data = reinterpret_cast<Adapter::AdapterItem*>(elm_object_item_data_get(obj));

	GenericList::ItemSignalEventInfo info = { data, eventData };

	eventItemSignal(this, info);
}

void TFC::Components::GenericList::SetLongClicked(const bool& o)
{
	longpressed = o;
}

bool TFC::Components::GenericList::GetLongClicked() const
{
	return longpressed;
}

void TFC::Components::GenericList::OnLongPressedInternal(
		Evas_Object* obj, void* eventData)
{
	if (isScrolling) {
		IsLongClicked = false;
	} else {
		IsLongClicked = true;
		Elm_Object_Item* temp1 = (Elm_Object_Item*) eventData;
		auto data = reinterpret_cast<GenlistItemClassPackage*>(elm_object_item_data_get(temp1));

		eventItemLongClicked(data->genlist, data->data);
	}
}

bool TFC::Components::GenericListItemClassBase::IsItemClickEnabled() const
{
	return this->itemClickEnabled;
}

LIBAPI
void TFC::Components::GenericList::ScrollToBottom()
{
	/*
	int v = 0, h = 0;
	elm_scroller_last_page_get(this->genlist, &h, &v);
	elm_scroller_page_bring_in(this->genlist, h, v);
	*/
	if(!this->dummyBottom)
	{
		auto lastItem = elm_genlist_last_item_get(this->genlist);
		elm_genlist_item_bring_in(lastItem, Elm_Genlist_Item_Scrollto_Type::ELM_GENLIST_ITEM_SCROLLTO_IN);
	}
	else
	{
		auto lastItem = elm_genlist_item_prev_get(this->dummyBottom);
		elm_genlist_item_bring_in(lastItem, Elm_Genlist_Item_Scrollto_Type::ELM_GENLIST_ITEM_SCROLLTO_IN);
	}
}

LIBAPI
TFC::Components::GenericList::ListItem TFC::Components::GenericList::operator [](Adapter::AdapterItem const& item)
{
	return { this->itemIndex.at(&item) };
}

LIBAPI
TFC::Components::GenericList::ListItem::operator Elm_Object_Item*()
{
	return item;
}

LIBAPI
TFC::Components::GenericList::ListItem::ListItem(Elm_Object_Item* obj) :
	item(obj),
	Selected(this)
{
}

LIBAPI
void TFC::Components::GenericList::ListItem::Update()
{
	elm_genlist_item_update(item);
}


void TFC::Components::GenericList::ListItem::SetSelected(const bool& sel)
{
	elm_genlist_item_selected_set(item, sel);
}

bool TFC::Components::GenericList::ListItem::GetSelected() const
{
	return elm_genlist_item_selected_get(item);
}
