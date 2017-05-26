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
 *    Infrastructures/ScheduledEvent.cpp
 *
 * Created on:  Sep 22, 2016
 * Author: 		Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Infrastructures/ScheduledEvent.h"

TFC::Infrastructures::ScheduledEvent::ScheduledEvent() :
	data(nullptr)
{
}

void TFC::Infrastructures::ScheduledEvent::SetData(void* data)
{
	this->data = data;
}

void TFC::Infrastructures::ScheduledEvent::Run()
{
	eventRunTask(this, data);
}
