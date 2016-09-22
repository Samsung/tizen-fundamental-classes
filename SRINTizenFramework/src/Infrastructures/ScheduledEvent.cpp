/*
 * ScheduledEvent.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: Kevin Winata
 */

#include "SRIN/Infrastructures/ScheduledEvent.h"

void SRIN::Infrastructures::ScheduledEvent::Run()
{
	eventRunTask(this, nullptr);
}
