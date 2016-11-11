/*
 * ScheduledEvent.cpp
 *
 *  Created on: Sep 22, 2016
 *      Contributor:
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Infrastructures/ScheduledEvent.h"

void TFC::Infrastructures::ScheduledEvent::Run()
{
	eventRunTask(this, nullptr);
}
