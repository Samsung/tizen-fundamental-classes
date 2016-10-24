/*
 * ScheduledEvent.h
 *
 *  Created on: Sep 22, 2016
 *      Contributor:
 *        Kevin Winata (k.winata@samsung.com)
 */

#ifndef TFC_INFRASTRUCTURES_SCHEDULEDEVENT_H_
#define TFC_INFRASTRUCTURES_SCHEDULEDEVENT_H_

#include "TFC/Infrastructures/ScheduledTask.h"

namespace TFC {
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


#endif /* TFC_INFRASTRUCTURES_SCHEDULEDEVENT_H_ */
