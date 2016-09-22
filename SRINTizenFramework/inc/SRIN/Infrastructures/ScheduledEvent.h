/*
 * ScheduledEvent.h
 *
 *  Created on: Sep 22, 2016
 *      Author: Kevin Winata
 */

#ifndef SRIN_INFRASTRUCTURES_SCHEDULEDEVENT_H_
#define SRIN_INFRASTRUCTURES_SCHEDULEDEVENT_H_

#include "SRIN/Infrastructures/ScheduledTask.h"

namespace SRIN {
namespace Infrastructures {

class LIBAPI ScheduledEvent : public ScheduledTask, public EventClass
{
protected:
	virtual void Run();
public:
	Event<ScheduledEvent*, void*> eventRunTask;
};

}
}


#endif /* SRIN_INFRASTRUCTURES_SCHEDULEDEVENT_H_ */
