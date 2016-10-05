/*
 * SplashScreenController.h
 *
 *  Created on: Apr 11, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SPLASHSCREENCONTROLLER_H_
#define SPLASHSCREENCONTROLLER_H_

#include "View/SplashScreenView.h"

using namespace $(appName)::View;

namespace $(appName) {
	namespace Controller {
		class SplashScreenController: public SRIN::Framework::ControllerBase {
		private:
			// Define the view of the controller here
			SplashScreenView splashScreenView;

		public:
			// Controller must have constructor with this exact parameter
			SplashScreenController(SRIN::Framework::ControllerManager* m);

			// This method will be called when the controller is loaded by the application
			virtual void OnLoad(ObjectClass* param);

			// This method will be called before leaving the controller
			virtual ObjectClass* OnUnload();
		};
	}
}



#endif /* SPLASHSCREENCONTROLLER_H_ */
