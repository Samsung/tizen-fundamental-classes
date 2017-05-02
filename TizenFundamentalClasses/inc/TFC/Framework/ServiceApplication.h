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
 *    Framework/ControllerManager.h
 *
 * Base class for service application
 *
 * Created on: 	Sep 9, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef SERVICEAPPLICATION_H_
#define SERVICEAPPLICATION_H_

#include <tizen.h>
#include <service_app.h>
#include "TFC/Core.h"

namespace TFC { namespace Framework {

	class LIBAPI ServiceApplicationBase {
	protected:
		virtual bool ApplicationCreate() = 0;

		virtual void ApplicationTerminate();

		virtual void ApplicationControl(app_control_h app_control);

		/**
		 * Method that will be called by Tizen system for any Language Changed event received.
		 * Override this method to react to the event.
		 */
		virtual void LanguageChanged(app_event_info_h event_info, const char* locale);

		/**
		 * Method that will be called by Tizen system for any Orientation Changed event received.
		 * Override this method to react to the event.
		 */
		virtual void OrientationChanged(app_event_info_h event_info);

		/**
		 * Method that will be called by Tizen system for any Region Changed event received.
		 * Override this method to react to the event.
		 */
		virtual void RegionChanged(app_event_info_h event_info);

		/**
		 * Method that will be called by Tizen system for any Low Battery event received. Override
		 * this method to react to the event.
		 */
		virtual void LowBattery(app_event_info_h event_info);

		/**
		 * Method that will be called by Tizen system for any Low Memory event received. Override
		 * this method to react to the event.
		 */
		virtual void LowMemory(app_event_info_h event_info);
	public:
		ServiceApplicationBase(char const* packageName);
		virtual ~ServiceApplicationBase();

		static int Main(ServiceApplicationBase* appObj, int argc, char* argv[]);
	};

}}



#endif /* SERVICEAPPLICATION_H_ */
