/*
/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Components/Pager.cpp
 *
 * Created on:  Mar 30, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Components/Pager.h"

LIBAPI Evas_Object* TFC::Components::Pager::CreateComponent(Evas_Object* root)
{
	this->pagerBox = elm_box_add(root);
	evas_object_size_hint_weight_set(this->pagerBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(this->pagerBox, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_padding_set(pagerBox, 0, 12);

	this->pagerTop	  = elm_layout_add(root);
	elm_layout_theme_set(this->pagerTop, "pager", "base", "default");

	evas_object_size_hint_weight_set(this->pagerTop, EVAS_HINT_EXPAND, 0);
	evas_object_size_hint_align_set(this->pagerTop, EVAS_HINT_FILL, EVAS_HINT_FILL);
	//elm_object_signal_callback_add(this->pagerTop, "elm,action,click,next", "", EFL::EdjeSignalEventHandler, &eventClickSignal);
	//elm_object_signal_callback_add(this->pagerTop, "elm,action,click,prev", "", EFL::EdjeSignalEventHandler, &eventClickSignal);
	eventClickSignal.Bind(this->pagerTop, "elm,action,click,next", "");
	eventClickSignal.Bind(this->pagerTop, "elm,action,click,prev", "");
	evas_object_show(this->pagerTop);

	elm_box_pack_start(this->pagerBox, this->pagerTop);

	elm_box_pack_end(this->pagerBox, this->PagerContent);

	this->pagerBottom	  = elm_layout_add(root);
	elm_layout_theme_set(this->pagerBottom, "pager", "base", "default");

	evas_object_size_hint_weight_set(this->pagerBottom, EVAS_HINT_EXPAND, 0);
	evas_object_size_hint_align_set(this->pagerBottom, EVAS_HINT_FILL, EVAS_HINT_FILL);
	//elm_object_signal_callback_add(this->pagerBottom, "elm,action,click,next", "", EFL::EdjeSignalEventHandler, &eventClickSignal);
	//elm_object_signal_callback_add(this->pagerBottom, "elm,action,click,prev", "", EFL::EdjeSignalEventHandler, &eventClickSignal);
	eventClickSignal.Bind(this->pagerBottom, "elm,action,click,next", "");
	eventClickSignal.Bind(this->pagerBottom, "elm,action,click,prev", "");
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

	elm_object_signal_emit(this->pagerTop, "disable_prev", "TFC");
	elm_object_signal_emit(this->pagerBottom, "disable_prev", "TFC");
	elm_object_part_text_set(this->pagerTop, "page_info", txt.c_str());
	elm_object_part_text_set(this->pagerBottom, "page_info", txt.c_str());
	*/

	if(!this->UnderflowEnable)
	{
		this->currentIndex = 1;

		int currentIndex = this->currentIndex;
		int maxIndex = this->MaxIndex;

		std::string txt(std::to_string(currentIndex));
		txt.append("/");
		txt.append(std::to_string(maxIndex));

		elm_object_part_text_set(this->pagerTop, "page_info", txt.c_str());
		elm_object_part_text_set(this->pagerBottom, "page_info", txt.c_str());

		elm_object_signal_emit(this->pagerTop, "disable_prev_noalign", "TFC");
		elm_object_signal_emit(this->pagerBottom, "disable_prev_noalign", "TFC");

		eventNavigate(this, 1);
	}
	else
	{
		elm_object_signal_emit(this->pagerTop, "disable_prev", "TFC");
		elm_object_signal_emit(this->pagerBottom, "disable_prev", "TFC");
		elm_object_part_text_set(this->pagerTop, "page_info", this->UnderflowString->c_str());
		elm_object_part_text_set(this->pagerBottom, "page_info", this->UnderflowString->c_str());
		this->currentIndex = 0;
		eventUnderflow(this, nullptr);
	}

	return this->pagerBox;
}

Evas_Object* TFC::Components::Pager::GetPagerContent()
{
	return pagerContent;
}

int TFC::Components::Pager::GetCurrentIndex() const
{
	return currentIndex;
}

int TFC::Components::Pager::GetMaxIndex() const
{
	return maxIndex;
}

bool TFC::Components::Pager::GetUnderflowEnable() const
{
	return underflowEnable;
}

bool TFC::Components::Pager::GetOverflowEnable() const
{
	return overflowEnable;
}

const std::string& TFC::Components::Pager::GetUnderflowString() const
{
	return underflowString;
}

const std::string& TFC::Components::Pager::GetOverflowString() const
{
	return overflowString;
}

void TFC::Components::Pager::SetPagerContent(Evas_Object* content)
{
	this->pagerContent = content;
}

void TFC::Components::Pager::SetMaxIndex(const int& maxIndex)
{
	this->maxIndex = maxIndex;
}

void TFC::Components::Pager::SetUnderflowEnable(const bool& underflow)
{
	this->underflowEnable = underflow;
}

void TFC::Components::Pager::SetOverflowEnable(const bool& overflow)
{
	this->overflowEnable = overflow;
}

void TFC::Components::Pager::SetUnderflowString(const std::string& str)
{
	this->underflowString = str;
}

void TFC::Components::Pager::SetOverflowString(const std::string& str)
{
	this->overflowString = str;
}

LIBAPI TFC::Components::Pager::Pager() :
		currentIndex(1),
		maxIndex(1),
		pagerBox(nullptr),
		pagerTop(nullptr),
		pagerBottom(nullptr),
		pagerContent(nullptr),
		underflowEnable(true),
		overflowEnable(true),
		PagerContent(this),
		CurrentIndex(this),
		MaxIndex(this),
		UnderflowEnable(this),
		OverflowEnable(this),
		UnderflowString(this),
		OverflowString(this)
{
	this->eventClickSignal += EventHandler(Pager::OnClickSignal);
}

void TFC::Components::Pager::OnClickSignal(Evas_Object* source, EFL::EdjeSignalInfo signalInfo)
{
	//dlog_print(DLOG_DEBUG, LOG_TAG, "Pager event! %s", signalInfo.emission);

	if(!strcmp(signalInfo.emission, "elm,action,click,next"))
	{
		NextPage();
	}
	else
	{
		PrevPage();
	}
}

void TFC::Components::Pager::NextPage()
{
	int index = this->currentIndex;
	int maxIndex = this->MaxIndex;

	if(index <= maxIndex)
	{
		elm_object_signal_emit(this->pagerTop, "elm,state,default", "elm");
		elm_object_signal_emit(this->pagerBottom, "elm,state,default", "elm");

		std::string txt(std::to_string(index + 1));
		txt.append("/");
		txt.append(std::to_string(maxIndex));

		elm_object_part_text_set(this->pagerTop, "page_info", txt.c_str());
		elm_object_part_text_set(this->pagerBottom, "page_info", txt.c_str());

		if(index == maxIndex)
		{
			// This is entering overflow
			elm_object_signal_emit(this->pagerTop, "disable_next", "TFC");
			elm_object_signal_emit(this->pagerBottom, "disable_next", "TFC");
			elm_object_part_text_set(this->pagerTop, "page_info", this->OverflowString->c_str());
			elm_object_part_text_set(this->pagerBottom, "page_info", this->OverflowString->c_str());
			eventOverflow(this, nullptr);
		}
		else if(index == maxIndex - 1 && !this->OverflowEnable)
		{
			// This is entering end of page
			elm_object_signal_emit(this->pagerTop, "disable_next_noalign", "TFC");
			elm_object_signal_emit(this->pagerBottom, "disable_next_noalign", "TFC");
			eventNavigate(this, index + 1);
		}
		else
			eventNavigate(this, index + 1);
		this->currentIndex = index + 1;
	}
}

void TFC::Components::Pager::PrevPage()
{
	int index = this->CurrentIndex;
	int maxIndex = this->MaxIndex;

	if(index > 0)
	{
		elm_object_signal_emit(this->pagerTop, "elm,state,default", "elm");
		elm_object_signal_emit(this->pagerBottom, "elm,state,default", "elm");

		std::string txt(std::to_string(index - 1));
		txt.append("/");
		txt.append(std::to_string(maxIndex));

		elm_object_part_text_set(this->pagerTop, "page_info", txt.c_str());
		elm_object_part_text_set(this->pagerBottom, "page_info", txt.c_str());

		if(index == 1)
		{
			// This is entering underflow
			elm_object_signal_emit(this->pagerTop, "disable_prev", "TFC");
			elm_object_signal_emit(this->pagerBottom, "disable_prev", "TFC");
			elm_object_part_text_set(this->pagerTop, "page_info", this->UnderflowString->c_str());
			elm_object_part_text_set(this->pagerBottom, "page_info", this->UnderflowString->c_str());
			eventUnderflow(this, nullptr);
		}
		else if(index == 2 && !this->UnderflowEnable)
		{
			// This is entering end of page
			elm_object_signal_emit(this->pagerTop, "disable_prev_noalign", "TFC");
			elm_object_signal_emit(this->pagerBottom, "disable_prev_noalign", "TFC");
			eventNavigate(this, index - 1);
		}
		else
			eventNavigate(this, index - 1);


		this->currentIndex = index - 1;
	}
}
