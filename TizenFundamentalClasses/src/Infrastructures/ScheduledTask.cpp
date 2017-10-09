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
 *    Infrastructures/ScheduledTask.cpp
 *
 * Created on:  Sep 15, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Infrastructures/ScheduledTask.h"

#include <stdexcept>


Eina_Bool TFC::Infrastructures::ScheduledTaskDispatcherCallback(void* data)
{
	return static_cast<TFC::Infrastructures::ScheduledTask*>(data)->TimerCallback() ? EINA_TRUE : EINA_FALSE;
}

LIBAPI TFC::Infrastructures::ScheduledTask::ScheduledTask() :
		timer(nullptr),
		interval(std::chrono::microseconds::zero()),
		firstStarted(false)
{

}

LIBAPI TFC::Infrastructures::ScheduledTask::~ScheduledTask() {
	Cancel();
}

bool TFC::Infrastructures::ScheduledTask::TimerCallback() {
	// Check if it is periodic
	auto isPeriodic = this->interval != std::chrono::microseconds::zero();

	if(isPeriodic && !this->firstStarted)
	{
		// If it is the first time it is started, recreate the new timer with the specifid interval
		std::chrono::duration<double, std::ratio<1>> intervalInDouble = this->interval;
		this->timer = ecore_timer_add(intervalInDouble.count(), ScheduledTaskDispatcherCallback, this);
		this->firstStarted = true;

		Run();

		return false;
	}
	else
	{
		// If it is not periodic, firstStarted will still be false
		// If it is periodic, and the second time, it will be true, which will renew the timer
		if(!this->firstStarted)
			this->timer = nullptr;

		Run();

		return this->firstStarted;
	}
}

LIBAPI void TFC::Infrastructures::ScheduledTask::ScheduleOnce(
		std::chrono::system_clock::time_point t) {
	SchedulePeriodic(t, std::chrono::microseconds(0));
}

LIBAPI void TFC::Infrastructures::ScheduledTask::SchedulePeriodic(
		std::chrono::system_clock::time_point startAt,
		std::chrono::microseconds interval) {
	if(this->timer != nullptr)
		throw std::runtime_error("Timer already scheduled");

	this->scheduledTime = startAt;
	this->firstStarted = false;
	this->interval = interval;

	std::chrono::duration<double, std::ratio<1>> durationToScheduledTime = startAt - std::chrono::system_clock::now();

	if(durationToScheduledTime.count() > 0)
	{
		this->timer = ecore_timer_add(durationToScheduledTime.count(), ScheduledTaskDispatcherCallback, this);
	}
	else
	{
		// Just start it now
		TimerCallback();
	}
}

LIBAPI void TFC::Infrastructures::ScheduledTask::Cancel() {
	if(this->timer == nullptr)
		return;

	ecore_timer_del(this->timer);
	this->timer = nullptr;
}





