/*
 * ScrollablePuller.cpp
 *
 *  Created on: Nov 18, 2016
 *      Author: Kevin Winata
 */

#include "TFC/Framework/Application.h"
#include "TFC/Components/ScrollablePuller.h"
#include <dlog.h>

#define FILE_EDC_CONTENT "TFC/edc/Content.edj"

LIBAPI
TFC::Components::ScrollablePuller::ScrollablePuller() :
	Scrollable(this),
	PartName(this),
	PullerSize(this),
	scrollable(nullptr),
	puller(nullptr),
	box(nullptr),
	bgTop(nullptr),
	bgBottom(nullptr),
	width(0),
	height(0),
	posX(0),
	posY(0),
	pullerShown(false),
	pullerSize(250)
{
	eventResize += EventHandler(ScrollablePuller::OnResize);
	eventScrollTop += EventHandler(ScrollablePuller::OnScrollTop);
	eventScrollDown += EventHandler(ScrollablePuller::OnScrollDown);
	eventPulledEnd += EventHandler(ScrollablePuller::OnPulledEnd);
	eventPullerScroll += EventHandler(ScrollablePuller::OnPullerScroll);
	eventPullerStop += EventHandler(ScrollablePuller::OnPullerStop);
}

LIBAPI
TFC::Components::ScrollablePuller::~ScrollablePuller()
{
	/*
	evas_object_event_callback_del(scrollable, EVAS_CALLBACK_RESIZE, EFL::EvasObjectEventHandler);
	evas_object_smart_callback_del(scrollable, "edge,top", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(scrollable, "scroll", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(puller, "scroll", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(puller, "scroll,drag,stop", EFL::EvasSmartEventHandler);
	evas_object_smart_callback_del(puller, "scroll,anim,stop", EFL::EvasSmartEventHandler);
	*/

	evas_object_del(puller);
}

LIBAPI
Evas_Object* TFC::Components::ScrollablePuller::CreateComponent(Evas_Object* root)
{
	if (scrollable == nullptr || partName.empty()) return root;

	rootLayout = root;
	/*
	evas_object_event_callback_add(scrollable, EVAS_CALLBACK_RESIZE, EFL::EvasObjectEventHandler, &eventResize);
	evas_object_smart_callback_add(scrollable, "edge,top", EFL::EvasSmartEventHandler, &eventScrollTop);
	evas_object_smart_callback_add(scrollable, "scroll", EFL::EvasSmartEventHandler, &eventScrollDown);
	*/
	eventResize.Bind(scrollable, EVAS_CALLBACK_RESIZE);
	eventScrollTop.Bind(scrollable, "edge,top");
	eventScrollDown.Bind(scrollable, "scroll");

	puller = elm_scroller_add(rootLayout);
	evas_object_repeat_events_set(puller, EINA_TRUE);
	elm_scroller_policy_set(puller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	elm_scroller_movement_block_set(puller, ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL);
	/*
	evas_object_smart_callback_add(puller, "scroll", EFL::EvasSmartEventHandler, &eventPullerScroll);
	evas_object_smart_callback_add(puller, "scroll,drag,stop", EFL::EvasSmartEventHandler, &eventPulledEnd);
	evas_object_smart_callback_add(puller, "scroll,anim,stop", EFL::EvasSmartEventHandler, &eventPullerStop);
	*/
	eventPullerScroll.Bind(puller, "scroll");
	eventPulledEnd.Bind(puller, "scroll,drag,stop");
	eventPullerStop.Bind(puller, "scroll,anim,stop");

	dlog_print(DLOG_DEBUG, LOG_TAG, "Creating scrollable puller");

	return puller;
}

void TFC::Components::ScrollablePuller::OnResize(EFL::EvasEventSourceInfo objSource, void* event_data)
{
	evas_object_show(puller);
	elm_object_part_content_set(rootLayout, partName.c_str(), puller);
	pullerShown = true;

	evas_object_geometry_get(scrollable, &posX, &posY, &width, &height);
	dlog_print(DLOG_DEBUG, LOG_TAG, "Scrollable puller Resize : %d %d", width, height);

	box = elm_box_add(puller);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, 0);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_content_set(puller, box);
	evas_object_show(box);

	auto edjPath = TFC::Framework::ApplicationBase::GetResourcePath(FILE_EDC_CONTENT);
	auto layout = elm_layout_add(box);
	elm_layout_file_set(layout, edjPath.c_str(), "custom-background");
	evas_object_resize(layout, width, pullerSize);
	evas_object_show(layout);
	elm_box_pack_end(box, layout);

	bgTop = elm_bg_add(box);
	evas_object_color_set(bgTop, 0, 0, 0, 0);
	evas_object_size_hint_min_set(bgTop, width, pullerSize);
	elm_object_part_content_set(layout, "bg", bgTop);

	Evas_Object* content = nullptr;
	eventPullerContentFill(this, &content);
	elm_object_part_content_set(layout, "content", content);

	bgBottom = elm_bg_add(box);
	evas_object_color_set(bgBottom, 0, 0, 0, 0);
	evas_object_size_hint_min_set(bgBottom, width, height);
	evas_object_show(bgBottom);
	elm_box_pack_end(box, bgBottom);

	elm_scroller_region_show(puller, width / 2, height + pullerSize - 1, 1, 1);
}

void TFC::Components::ScrollablePuller::OnScrollTop(Evas_Object* obj, void* eventData)
{
	TogglePuller(true);
}

void TFC::Components::ScrollablePuller::OnScrollDown(Evas_Object* obj, void* eventData)
{
	TogglePuller(false);
}

void TFC::Components::ScrollablePuller::OnPulledEnd(Evas_Object* obj, void* eventData)
{
	Evas_Coord y;
	elm_scroller_region_get(puller, nullptr, &y, nullptr, nullptr);
	if (y == 0) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "Scrollable puller event trigger");
		eventPull(this, nullptr);
	}
	ecore_idler_add([] (void* data) -> Eina_Bool {
		//dlog_print(DLOG_DEBUG, "PULLER", "Puller reset");
		auto thisPtr = (ScrollablePuller*)(data);
		thisPtr->ResetPuller();
		return ECORE_CALLBACK_CANCEL;
	}, this);
}

void TFC::Components::ScrollablePuller::OnPullerScroll(Evas_Object* obj, void* eventData)
{
	//dlog_print(DLOG_DEBUG, "PULLER", "Puller scroll");
	Evas_Coord y;
	elm_scroller_region_get(puller, nullptr, &y, nullptr, nullptr);
	int alpha = (pullerSize - y) * 120 / pullerSize;
	evas_object_color_set(bgTop, 0, 0, 0, alpha);
	evas_object_color_set(bgBottom, 0, 0, 0, alpha);
}

void TFC::Components::ScrollablePuller::OnPullerStop(Evas_Object* obj, void* eventData)
{
	ResetPuller();
}

LIBAPI
void TFC::Components::ScrollablePuller::ResetPuller()
{
	elm_scroller_region_show(puller, width / 2, height + pullerSize - 1, 1, 1);
}

LIBAPI
void TFC::Components::ScrollablePuller::TogglePuller(bool enable)
{
	if (enable && !pullerShown) {
		//dlog_print(DLOG_DEBUG, "PULLER", "Adding puller");
		elm_object_part_content_set(rootLayout, partName.c_str(), puller);
		evas_object_show(puller);
		pullerShown = true;
		ResetPuller();
	}
	else if (!enable && pullerShown) {
		//if (!elm_object_part_content_get(rootLayout, partName.c_str())) {
		//dlog_print(DLOG_DEBUG, "PULLER", "Removing puller");
		elm_object_part_content_unset(rootLayout, partName.c_str());
		evas_object_hide(puller);
		pullerShown = false;
		//}
	}
}



