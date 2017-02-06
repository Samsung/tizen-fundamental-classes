/*
 * GenericGrid.cpp
 *
 *  Created on: Feb 2, 2017
 *      Author: azzam
 */

#include "TFC/Components/GenericGrid.h"
#include <dlog.h>

using namespace TFC::Components;

typedef struct
{
	GenericGridItemClassBase* base;
	GenericGrid* gengrid;
	void* data;
} GengridItemClassPackage;

void Gengrid_ClickedEventHandler(void* data, Evas_Object* obj, void* eventData)
{
	auto package = static_cast<GengridItemClassPackage*>(data);

	if (package->gengrid->IsLongClicked) {
		package->gengrid->IsLongClicked = false;
		//package->gengrid->ItemLongClicked(package->gengrid, package->data);
	} else {
		package->gengrid->eventItemClicked(package->gengrid, package->data);
	}
}


/* =================================================================================================================
 * IMPLEMENTATION: GenericGridItemClassBase
 * ================================================================================================================= */

LIBAPI TFC::Components::GenericGridItemClassBase::GenericGridItemClassBase(char const* styleName, bool defaultEventClick)
{
	itemClass = elm_gengrid_item_class_new();
	itemClass->item_style = styleName;

	this->itemClickEnabled = defaultEventClick;

	// Callback redirect for get text function
	itemClass->func.text_get = [] (void *data, Evas_Object *obj, const char *part) ->  char*
	{
		auto pkg = reinterpret_cast<GengridItemClassPackage*>(data);
		std::string ret = pkg->base->GetString(pkg->data, obj, part);
		return strdup(ret.c_str());
	};

	// Callback redirect for get content function
	itemClass->func.content_get = [] (void *data, Evas_Object *obj, const char *part) -> Evas_Object*
	{
		auto pkg = reinterpret_cast<GengridItemClassPackage*>(data);
		return pkg->base->GetContent(pkg->data, obj, part);
	};

	// Callback redirect for delete function
	itemClass->func.del = [] (void *data, Evas_Object *obj)
	{
		auto pkg = reinterpret_cast<GengridItemClassPackage*>(data);
		delete pkg;
	};
}

LIBAPI void* TFC::Components::GenericGridItemClassBase::operator ()(GenericGrid* gengrid, void* itemData)
{
	return new GengridItemClassPackage({this, gengrid, itemData}) ;
}

LIBAPI TFC::Components::GenericGridItemClassBase::operator Elm_Gengrid_Item_Class*()
{
	return this->itemClass;
}

LIBAPI TFC::Components::GenericGridItemClassBase::~GenericGridItemClassBase()
{
	if(this->itemClass)
	{
		elm_gengrid_item_class_free(this->itemClass);
		this->itemClass = nullptr;
	}
}



/* =================================================================================================================
 * IMPLEMENTATION: GenericGrid
 * ================================================================================================================= */

LIBAPI TFC::Components::GenericGrid::GenericGrid() :
	gengrid(nullptr), lastItem(nullptr), dataSource(nullptr), DataSource(this),
	IsLongClicked(this), IsHorizontalMode(this), Width(this), Height(this)
{
	eventScrolledInternal += EventHandler(GenericGrid::OnScrolledInternal);
	eventScrolledDownInternal += EventHandler(GenericGrid::OnScrolledDownInternal);
	eventScrolledUpInternal += EventHandler(GenericGrid::OnScrolledUpInternal);
	eventScrolledBottomInternal += EventHandler(GenericGrid::OnScrolledBottomInternal);
	eventScrolledTopInternal += EventHandler(GenericGrid::OnScrolledTopInternal);
	eventScrolledLeftInternal += EventHandler(GenericGrid::OnScrolledLeftInternal);
	eventScrolledRightInternal += EventHandler(GenericGrid::OnScrolledRightInternal);
	eventLongPressedInternal += EventHandler(GenericGrid::OnLongPressedInternal);
	eventScrollingStartInternal += EventHandler(GenericGrid::OnScrollingStart);
	eventScrollingEndInternal += EventHandler(GenericGrid::OnScrollingEnd);
	eventItemSignalInternal += EventHandler(GenericGrid::OnItemSignalEmit);

	isScrolling = false;
	longpressed = false;
	horizontalMode=false;
	itemWidth=70;
	itemHeight=70;
	lastItem = nullptr;
	firstItem = nullptr;
}

LIBAPI TFC::Components::GenericGrid::~GenericGrid()
{
}

LIBAPI void TFC::Components::GenericGrid::ResetScroll(bool animated)
{
	if (animated) {
		elm_scroller_page_bring_in(gengrid, 0, 0);
	}
	else {
		elm_scroller_page_show(gengrid, 0, 0);
	}
}


LIBAPI void TFC::Components::GenericGrid::SetDataSource(Adapter* newAdapter)
{
	// Unbind the old adapter
	if(dataSource != nullptr)
	{
		auto oldGrid = dataSource->GetAll();
		for(auto& item : oldGrid)
		{
			// Remove all item
			elm_object_item_del(item.objectItem);
			item.objectItem = nullptr;
		}

		// Remove event
		dataSource->eventItemAdd -= EventHandler(GenericGrid::OnItemAdd);
		dataSource->eventItemRemove -= EventHandler(GenericGrid::OnItemRemove);
	}

	// Assign new adapter
	this->dataSource = newAdapter;
	// Append all existing item in adapter
	auto all = dataSource->GetAll();
	for(auto& item : all)
	{
		AppendItemToGengrid(&item);
	}
	// Attach to event
	dataSource->eventItemAdd += EventHandler(GenericGrid::OnItemAdd);
	dataSource->eventItemRemove += EventHandler(GenericGrid::OnItemRemove);
}

LIBAPI void TFC::Components::GenericGrid::AppendItemToGengrid(Adapter::AdapterItem* data)
{
	auto itemClass = dynamic_cast<GenericGridItemClassBase*>(data->itemClass);
	if(itemClass == nullptr)
		throw std::runtime_error("Invalid item class specified for generic grid. Use GenericGridItemClass type");

	auto package = (*(itemClass))(this, data->data);

	decltype(Gengrid_ClickedEventHandler)* handlerPtr = nullptr;
	void* eventPackage = nullptr;

	if(itemClass->IsItemClickEnabled())
	{
		handlerPtr = &Gengrid_ClickedEventHandler;
		eventPackage = package;
	}

	if (elm_gengrid_items_count(gengrid) == 0)
	{
		firstItem = lastItem = elm_gengrid_item_append(gengrid, *(itemClass), package,  handlerPtr, eventPackage);
	}else{
		lastItem = elm_gengrid_item_append(gengrid, *(itemClass), package, handlerPtr, eventPackage);
	}
	data->objectItem = lastItem;




	elm_object_item_signal_callback_add(lastItem, "*", "*", [] (void *data, Evas_Object *obj, const char *emission, const char *source) {
			dlog_print(DLOG_DEBUG, "TFCFW-Signal", "Signal TFC %s, source %s", emission, source);
	}, nullptr);
}

LIBAPI void TFC::Components::GenericGrid::OnItemAdd(Adapter* adapter, Adapter::AdapterItem* data)
{
	AppendItemToGengrid(data);
}

LIBAPI void TFC::Components::GenericGrid::OnItemRemove(Adapter* adapter, Adapter::AdapterItem* data)
{
	if(data->objectItem == lastItem)
		lastItem = elm_gengrid_item_prev_get(lastItem);

	elm_object_item_del(data->objectItem);
	data->objectItem = nullptr;

	if(adapter->GetCount() == 1)
	{
		lastItem = firstItem;

	}
}

LIBAPI Evas_Object* TFC::Components::GenericGrid::CreateComponent(Evas_Object* root)
{
	gengrid = elm_gengrid_add(root);

	eventScrolledInternal.Bind(gengrid, "scroll");
	eventScrolledDownInternal.Bind(gengrid, "scroll,down");
	eventScrolledUpInternal.Bind(gengrid, "scroll,up");
	eventScrolledTopInternal.Bind(gengrid, "edge,top");
	eventScrolledBottomInternal.Bind(gengrid, "edge,bottom");
	eventScrolledLeftInternal.Bind(gengrid, "edge,left");
	eventScrolledRightInternal.Bind(gengrid, "edge,right");
	eventScrollingStartInternal.Bind(gengrid, "scroll,drag,start");
	eventScrollingEndInternal.Bind(gengrid, "scroll,drag,stop");
	eventLongPressedInternal.Bind(gengrid, "longpressed");

	elm_gengrid_highlight_mode_set(gengrid, EINA_FALSE);
	elm_gengrid_select_mode_set(gengrid, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_gengrid_align_set(gengrid, 0.5, 0.5);
	elm_gengrid_horizontal_set(gengrid, horizontalMode);
	elm_gengrid_item_size_set(gengrid, (int)(ELM_SCALE_SIZE(itemWidth)), (int)(ELM_SCALE_SIZE(itemHeight)));

	return gengrid;
}

LIBAPI TFC::Components::Adapter* TFC::Components::GenericGrid::GetDataSource()
{
	return dataSource;
}

void TFC::Components::GenericGrid::OnScrolledInternal(Evas_Object* obj, void* eventData)
{

	eventScrolled(this, eventData);
}

void TFC::Components::GenericGrid::OnScrolledDownInternal(Evas_Object* obj, void* eventData)
{
	eventScrolledDown(this, eventData);
}

void TFC::Components::GenericGrid::OnScrolledUpInternal(Evas_Object* obj, void* eventData)
{
	eventScrolledUp(this, eventData);
}

void TFC::Components::GenericGrid::OnScrolledBottomInternal(Evas_Object* obj, void* eventData)
{
	eventScrolledBottom(this, eventData);
}

void TFC::Components::GenericGrid::OnScrolledTopInternal(Evas_Object* obj, void* eventData)
{
	eventScrolledTop(this, eventData);
}

void TFC::Components::GenericGrid::OnScrolledLeftInternal(Evas_Object* obj, void* eventData){
	eventScrolledLeft(this, eventData);
}

void TFC::Components::GenericGrid::OnScrolledRightInternal(Evas_Object* obj, void* eventData){
	eventScrolledRight(this, eventData);
}


void TFC::Components::GenericGrid::OnScrollingStart(Evas_Object* obj, void* eventData)
{
	isScrolling   = true;
	IsLongClicked = false;
	eventScrollingStart(this, nullptr);
}

void TFC::Components::GenericGrid::OnScrollingEnd(Evas_Object* obj, void* eventData)
{
	isScrolling = false;
	eventScrollingEnd(this, nullptr);
}

void TFC::Components::GenericGrid::OnItemSignalEmit(Elm_Object_Item* obj, EFL::EdjeSignalInfo eventData)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Signal %s", eventData.source);

	auto data = reinterpret_cast<Adapter::AdapterItem*>(elm_object_item_data_get(obj));

	GenericGrid::ItemSignalEventInfo info = { data, eventData };

	eventItemSignal(this, info);
}

void TFC::Components::GenericGrid::SetLongClicked(const bool& o) {
	longpressed = o;
}

bool TFC::Components::GenericGrid::GetLongClicked() const {
	return longpressed;
}

void  TFC::Components::GenericGrid::SetHorizontalMode(bool const& o){
	horizontalMode=o;
}

bool  TFC::Components::GenericGrid::GetHorizontalMode() const{
	return horizontalMode;
}

void TFC::Components::GenericGrid::SetWidth(int const& o){
	itemWidth=o;
}

int TFC::Components::GenericGrid::GetWidth() const{
	return itemWidth;
}

void TFC::Components::GenericGrid::SetHeight(int const& o){
	itemHeight=o;
}

int TFC::Components::GenericGrid::GetHeight() const{
	return itemHeight;
}

void TFC::Components::GenericGrid::OnLongPressedInternal(
		Evas_Object* obj, void* eventData) {
	if (isScrolling) {
		IsLongClicked = false;
	} else {
		IsLongClicked = true;
		Elm_Object_Item* temp1 = (Elm_Object_Item*) eventData;
		auto data = reinterpret_cast<GengridItemClassPackage*>(elm_object_item_data_get(temp1));

		eventItemLongClicked(data->gengrid, data->data);
	}
}

bool TFC::Components::GenericGridItemClassBase::IsItemClickEnabled() const {
	return this->itemClickEnabled;
}
