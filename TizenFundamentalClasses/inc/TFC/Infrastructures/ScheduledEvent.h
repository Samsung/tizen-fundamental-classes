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
 *    Infrastructures/ScheduledEvent.h
 *
 * Scheduled task with event handling mechanism
 *
 * Created on: 	Sep 22, 2016
 * Author: 		Kevin Winata (k.winata@samsung.com)
 */

#ifndef TFC_INFRASTRUCTURES_SCHEDULEDEVENT_H_
#define TFC_INFRASTRUCTURES_SCHEDULEDEVENT_H_

#include "TFC/Infrastructures/ScheduledTask.h"
#include <functional>

namespace TFC {
namespace Infrastructures {

class LIBAPI ScheduledEvent : public ScheduledTask, public EventEmitterClass<ScheduledEvent>
{
private:
	void* data;
protected:
	virtual void Run();
public:
	ScheduledEvent();
	void SetData(void* data);
	Event<void*> eventRunTask;
};

}
}


#endif /* TFC_INFRASTRUCTURES_SCHEDULEDEVENT_H_ */
