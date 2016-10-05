/*
 * ScheduledEvent.cpp
 *
 *  Created on: Sep 22, 2016
 *      Contributor:
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "SRIN/Infrastructures/ScheduledEvent.h"

void SRIN::Infrastructures::ScheduledEvent::Run()
{
	eventRunTask(this, nullptr);
}
