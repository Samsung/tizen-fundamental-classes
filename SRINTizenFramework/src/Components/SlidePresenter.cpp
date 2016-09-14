/*
 * SlidePresenter.cpp
 *
 *  Created on: Sep 14, 2016
 *      Author: Gilang
 */

#include "SRIN/Components/SlidePresenter.h"

#define FILE_EDC_SLIDEPRESENTER "SRIN/edc/SlidePresenter.edj"

void SRIN::Components::SlidePresenter::OnLayoutResize(
		EFL::EvasObjectEvent* event, EFL::EvasEventSourceInfo* objSource,
		void* event_data) {
}

void SRIN::Components::SlidePresenter::OnPageScrolled(
		EFL::EvasSmartEvent* event, Evas_Object* source, void* event_data) {
}

Evas_Object* SRIN::Components::SlidePresenter::CreateComponent(
		Evas_Object* root) {
	auto layout = elm_layout_add(root);
	auto edj_path = Framework::ApplicationBase::GetResourcePath(FILE_EDC_SLIDEPRESENTER);
	elm_layout_file_set(layout, edj_path.c_str(), "slide_presenter");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_event_callback_add(layout, EVAS_CALLBACK_RESIZE, EFL::EvasObjectEventHandler, &this->eventLayoutResize);

	this->scroller = elm_scroller_add(layout);
	elm_scroller_loop_set(this->scroller, EINA_FALSE, EINA_FALSE);
	evas_object_size_hint_weight_set(this->scroller, 0.5, 0.5);
	evas_object_size_hint_align_set(this->scroller, 0.5, 0.5);
	elm_scroller_page_relative_set(this->scroller, 1.0, 0.0);
	elm_scroller_policy_set(this->scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	elm_scroller_page_scroll_limit_set(this->scroller, 1, 0);
	elm_object_scroll_lock_y_set(this->scroller, EINA_TRUE);
	evas_object_size_hint_weight_set(this->scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(this->scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(this->scroller);
	evas_object_smart_callback_add(this->scroller, "scroll,page,changed", EFL::EvasSmartEventHandler, &this->eventPageScrolled);

	elm_object_part_content_set(layout, "scroller", this->scroller);

	// Box inside scroller
	this->box = elm_box_add(this->scroller);
	elm_box_horizontal_set(this->box, EINA_TRUE);
	elm_object_content_set(this->scroller, this->box);
	evas_object_size_hint_weight_set(this->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(this->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(this->box);
	elm_box_homogeneous_set(this->box, EINA_TRUE);

	// Index page
	this->index = elm_index_add(layout);
	evas_object_size_hint_weight_set(this->index, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(this->index, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_style_set(this->index, "pagecontrol");
	elm_index_horizontal_set(this->index, EINA_TRUE);
	elm_index_autohide_disabled_set(this->index, EINA_TRUE);
	elm_object_part_content_set(layout, "index", this->index);

	return layout;
}

SRIN::Components::SlidePresenter::SlidePresenter() :
	box(nullptr), scroller(nullptr), index(nullptr){

	this->eventLayoutResize += EventHandler(SlidePresenter::OnLayoutResize);
	this->eventPageScrolled += EventHandler(SlidePresenter::OnPageScrolled);
}

SRIN::Components::SlidePresenter::~SlidePresenter() {
}


void SRIN::Components::SlidePresenter::InsertPage(Evas_Object* page) {
}

void SRIN::Components::SlidePresenter::InsertPageAt(Evas_Object* page,
		int index) {
}

void SRIN::Components::SlidePresenter::Remove(int index) {
}
