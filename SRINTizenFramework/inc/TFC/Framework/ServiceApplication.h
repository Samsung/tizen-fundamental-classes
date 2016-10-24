/*
 * ServiceApplication.h
 *
 *  Created on: Sep 9, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
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
		ServiceApplicationBase(CString packageName);
		virtual ~ServiceApplicationBase();

		static int Main(ServiceApplicationBase* appObj, int argc, char* argv[]);
	};

}}



#endif /* SERVICEAPPLICATION_H_ */
