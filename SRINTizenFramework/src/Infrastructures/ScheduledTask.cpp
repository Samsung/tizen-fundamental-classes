/*
 * ScheduledTask.cpp
 *
 *  Created on: Sep 15, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "SRIN/Infrastructures/ScheduledTask.h"

#include <stdexcept>


Eina_Bool SRIN::Infrastructures::ScheduledTaskDispatcherCallback(void* data)
{
	return static_cast<SRIN::Infrastructures::ScheduledTask*>(data)->TimerCallback() ? EINA_TRUE : EINA_FALSE;
}

LIBAPI SRIN::Infrastructures::ScheduledTask::ScheduledTask() :
		timer(nullptr),
		interval(std::chrono::microseconds::zero()),
		firstStarted(false)
{

}

LIBAPI SRIN::Infrastructures::ScheduledTask::~ScheduledTask() {
	Cancel();
}

bool SRIN::Infrastructures::ScheduledTask::TimerCallback() {
	Run();

	// Check if it is periodic
	auto isPeriodic = this->interval != std::chrono::microseconds::zero();
	if(isPeriodic && !this->firstStarted)
	{
		// If it is the first time it is started, recreate the new timer with the specifid interval
		std::chrono::duration<double, std::ratio<1>> intervalInDouble = this->interval;
		this->timer = ecore_timer_add(intervalInDouble.count(), ScheduledTaskDispatcherCallback, this);
		this->firstStarted = true;
		return false;
	}

	// If it is not periodic, firstStarted will still be false
	// If it is periodic, and the second time, it will be true, which will renew the timer
	return this->firstStarted;
}

LIBAPI void SRIN::Infrastructures::ScheduledTask::ScheduleOnce(
		std::chrono::system_clock::time_point t) {
	SchedulePeriodic(t, std::chrono::microseconds(0));
}

LIBAPI void SRIN::Infrastructures::ScheduledTask::SchedulePeriodic(
		std::chrono::system_clock::time_point startAt,
		std::chrono::microseconds interval) {
	if(this->timer != nullptr)
		throw std::runtime_error("Timer already scheduled");

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

LIBAPI void SRIN::Infrastructures::ScheduledTask::Cancel() {
	if(this->timer == nullptr)
		return;

	ecore_timer_del(this->timer);
}





