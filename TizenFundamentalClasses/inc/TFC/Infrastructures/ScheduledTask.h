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
 *    Infrastructures/ScheduledTask.h
 *
 * Class to perform task on scheduled time
 *
 * Created on: 	Sep 13, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "TFC/Core.h"
#include <Ecore.h>
#include <chrono>

namespace TFC {
namespace Infrastructures {

class LIBAPI ScheduledTask
{
private:
	Ecore_Timer* timer;
	bool TimerCallback();
	std::chrono::microseconds interval;
	bool firstStarted;
protected:
	virtual void Run() = 0;
public:
	friend Eina_Bool ScheduledTaskDispatcherCallback(void* data);

	ScheduledTask();
	virtual ~ScheduledTask();
	void ScheduleOnce(std::chrono::system_clock::time_point t);
	void SchedulePeriodic(std::chrono::system_clock::time_point startAt, std::chrono::microseconds interval);
	void Cancel();
	bool IsScheduled() { return timer != nullptr; }
};

}
}



#endif /* SCHEDULER_H_ */
