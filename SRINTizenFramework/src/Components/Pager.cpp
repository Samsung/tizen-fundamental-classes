/*
 * Pager.cpp
 *
 *  Created on: Mar 30, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Components/Pager.h"

LIBAPI Evas_Object* SRIN::Components::Pager::CreateComponent(Evas_Object* root)
{
	this->pagerBox = elm_box_add(root);
	evas_object_size_hint_weight_set(this->pagerBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(this->pagerBox, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_padding_set(pagerBox, 0, 12);

	this->pagerTop	  = elm_layout_add(root);
	elm_layout_theme_set(this->pagerTop, "pager", "base", "default");

	evas_object_size_hint_weight_set(this->pagerTop, EVAS_HINT_EXPAND, 0);
	evas_object_size_hint_align_set(this->pagerTop, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_signal_callback_add(this->pagerTop, "elm,action,click,next", "", EFL::EdjeSignalEventHandler, &eventClickSignal);
	elm_object_signal_callback_add(this->pagerTop, "elm,action,click,prev", "", EFL::EdjeSignalEventHandler, &eventClickSignal);
	evas_object_show(this->pagerTop);

	elm_box_pack_start(this->pagerBox, this->pagerTop);

	elm_box_pack_end(this->pagerBox, this->PagerContent);

	this->pagerBottom	  = elm_layout_add(root);
	elm_layout_theme_set(this->pagerBottom, "pager", "base", "default");

	evas_object_size_hint_weight_set(this->pagerBottom, EVAS_HINT_EXPAND, 0);
	evas_object_size_hint_align_set(this->pagerBottom, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_signal_callback_add(this->pagerBottom, "elm,action,click,next", "", EFL::EdjeSignalEventHandler, &eventClickSignal);
	elm_object_signal_callback_add(this->pagerBottom, "elm,action,click,prev", "", EFL::EdjeSignalEventHandler, &eventClickSignal);
	evas_object_show(this->pagerBottom);

	elm_box_pack_end(this->pagerBox, this->pagerBottom);

	/*
	this->multiplePrevButton	= elm_button_add(this->multipleLayout);
	elm_layout_file_set(this->multiplePrevButton, edjPath->c_str(), "button_icon_prev");
	evas_object_size_hint_weight_set(this->multiplePrevButton, 0, 0);
	evas_object_size_hint_align_set(this->multiplePrevButton, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(this->multiplePrevButton);

	this->multipleNextButton	= elm_button_add(this->multipleLayout);
	elm_layout_file_set(this->multipleNextButton, edjPath->c_str(), "button_icon_next");
	evas_object_size_hint_weight_set(this->multipleNextButton, 0, 0);
	evas_object_size_hint_align_set(this->multipleNextButton, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(this->multipleNextButton);

	elm_object_part_content_set(this->multipleLayout, "container_prev", this->multiplePrevButton);
	elm_object_part_content_set(this->multipleLayout, "container_next", this->multipleNextButton);
	*/

	/*
	int currentIndex = this->CurrentIndex;
	int maxIndex = this->MaxIndex;

	std::string txt(std::to_string(currentIndex));
	txt.append("/");
	txt.append(std::to_string(maxIndex));

	elm_object_signal_emit(this->pagerTop, "disable_prev", "srin");
	elm_object_signal_emit(this->pagerBottom, "disable_prev", "srin");
	elm_object_part_text_set(this->pagerTop, "page_info", txt.c_str());
	elm_object_part_text_set(this->pagerBottom, "page_info", txt.c_str());
	*/

	if(!this->UnderflowEnable)
	{
		this->CurrentIndex = 1;

		int currentIndex = this->CurrentIndex;
		int maxIndex = this->MaxIndex;

		std::string txt(std::to_string(currentIndex));
		txt.append("/");
		txt.append(std::to_string(maxIndex));

		elm_object_part_text_set(this->pagerTop, "page_info", txt.c_str());
		elm_object_part_text_set(this->pagerBottom, "page_info", txt.c_str());

		elm_object_signal_emit(this->pagerTop, "disable_prev_noalign", "srin");
		elm_object_signal_emit(this->pagerBottom, "disable_prev_noalign", "srin");

		eventNavigate(this, 1);
	}
	else
	{
		elm_object_signal_emit(this->pagerTop, "disable_prev", "srin");
		elm_object_signal_emit(this->pagerBottom, "disable_prev", "srin");
		elm_object_part_text_set(this->pagerTop, "page_info", this->UnderflowString->c_str());
		elm_object_part_text_set(this->pagerBottom, "page_info", this->UnderflowString->c_str());
		this->CurrentIndex = 0;
		eventUnderflow(this, nullptr);
	}

	return this->pagerBox;
}

LIBAPI SRIN::Components::Pager::Pager()
{
	this->pagerTop = nullptr;
	this->pagerBottom = nullptr;
	this->pagerBox = nullptr;
	this->PagerContent = nullptr;

	this->MaxIndex = 1;
	this->CurrentIndex = 1;
	this->UnderflowEnable = true;
	this->OverflowEnable = true;

	this->eventClickSignal += AddEventHandler(Pager::OnClickSignal);
}

void SRIN::Components::Pager::OnClickSignal(EFL::EdjeSignalEvent* event, Evas_Object* source, EFL::EdjeSignalInfo signalInfo)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Pager event! %s", signalInfo.emission);

	if(!strcmp(signalInfo.emission, "elm,action,click,next"))
	{
		NextPage();
	}
	else
	{
		PrevPage();
	}
}

void SRIN::Components::Pager::NextPage()
{
	int currentIndex = this->CurrentIndex;
	int maxIndex = this->MaxIndex;

	if(currentIndex <= maxIndex)
	{
		elm_object_signal_emit(this->pagerTop, "elm,state,default", "elm");
		elm_object_signal_emit(this->pagerBottom, "elm,state,default", "elm");

		std::string txt(std::to_string(currentIndex + 1));
		txt.append("/");
		txt.append(std::to_string(maxIndex));

		elm_object_part_text_set(this->pagerTop, "page_info", txt.c_str());
		elm_object_part_text_set(this->pagerBottom, "page_info", txt.c_str());

		if(currentIndex == maxIndex)
		{
			// This is entering overflow
			elm_object_signal_emit(this->pagerTop, "disable_next", "srin");
			elm_object_signal_emit(this->pagerBottom, "disable_next", "srin");
			elm_object_part_text_set(this->pagerTop, "page_info", this->OverflowString->c_str());
			elm_object_part_text_set(this->pagerBottom, "page_info", this->OverflowString->c_str());
			eventOverflow(this, nullptr);
		}
		else if(currentIndex == maxIndex - 1 && !this->OverflowEnable)
		{
			// This is entering end of page
			elm_object_signal_emit(this->pagerTop, "disable_next_noalign", "srin");
			elm_object_signal_emit(this->pagerBottom, "disable_next_noalign", "srin");
			eventNavigate(this, currentIndex + 1);
		}
		else
			eventNavigate(this, currentIndex + 1);
		this->CurrentIndex = currentIndex + 1;
	}
}

void SRIN::Components::Pager::PrevPage()
{
	int currentIndex = this->CurrentIndex;
	int maxIndex = this->MaxIndex;

	if(currentIndex > 0)
	{
		elm_object_signal_emit(this->pagerTop, "elm,state,default", "elm");
		elm_object_signal_emit(this->pagerBottom, "elm,state,default", "elm");

		std::string txt(std::to_string(currentIndex - 1));
		txt.append("/");
		txt.append(std::to_string(maxIndex));

		elm_object_part_text_set(this->pagerTop, "page_info", txt.c_str());
		elm_object_part_text_set(this->pagerBottom, "page_info", txt.c_str());

		if(currentIndex == 1)
		{
			// This is entering underflow
			elm_object_signal_emit(this->pagerTop, "disable_prev", "srin");
			elm_object_signal_emit(this->pagerBottom, "disable_prev", "srin");
			elm_object_part_text_set(this->pagerTop, "page_info", this->UnderflowString->c_str());
			elm_object_part_text_set(this->pagerBottom, "page_info", this->UnderflowString->c_str());
			eventUnderflow(this, nullptr);
		}
		else if(currentIndex == 2 && !this->UnderflowEnable)
		{
			// This is entering end of page
			elm_object_signal_emit(this->pagerTop, "disable_prev_noalign", "srin");
			elm_object_signal_emit(this->pagerBottom, "disable_prev_noalign", "srin");
			eventNavigate(this, currentIndex - 1);
		}
		else
			eventNavigate(this, currentIndex - 1);


		this->CurrentIndex = currentIndex - 1;
	}
}
