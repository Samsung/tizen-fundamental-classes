/*
 * SplashScreenController.cpp
 *
 *  Created on: Apr 11, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "Controller/SplashScreenController.h"


$(appName)::Controller::SplashScreenController::SplashScreenController(SRIN::Framework::ControllerManager* m) :
	SRIN::Framework::ControllerBase( /* Call base constructor to pass information to it */
		m, 						/* Pass the parameter to parent */
		&splashScreenView, 		/* The view for this controller */
		CONTROLLER_SPLASHSCREEN /* The name of this controller */)
{
}

void $(appName)::Controller::SplashScreenController::OnLoad(ObjectClass* param)
{
	// The view can expose property of its internal component and we can assign it like we assign a variable
	splashScreenView.FieldText = "Hello, I am accessed through the controller!";
}

ObjectClass* $(appName)::Controller::SplashScreenController::OnUnload()
{
	// Return anything to previous controller who load this controller
	return nullptr;
}
