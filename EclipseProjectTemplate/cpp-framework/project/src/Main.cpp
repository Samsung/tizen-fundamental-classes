/*
 * $(appName).cpp
 *	
 *	Main implementation for native application utilizing SRIN C++ Framework
 *
 *  Template Created on: Apr 11, 2016
 *      		 Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "$(appName).h"
#include "Controller/SplashScreenController.h"

$(appName)AppClass $(appName)App;

$(appName)AppClass::$(appName)AppClass() :
	SRIN::Framework::MVCApplicationBase(PACKAGE, /* Start up controller */ CONTROLLER_SPLASHSCREEN)
{
	// Registrer controller class here
	RegisterController(CONTROLLER_SPLASHSCREEN, /* Controller name definition*/
					   $(appName)::Controller::SplashScreenController /* Class of the controller */);
}

void $(appName)AppClass::OnApplicationCreated()
{
	// This method is called after the application is created but before the first controller is loaded
	// Alwasy call base class implementation when overriding this method
	MVCApplicationBase::OnApplicationCreated();
}

int main(int argc, char** argv)
{
	// Always call Framework Main
	return SRIN::Framework::ApplicationBase::Main(&$(appName)App, argc, argv);
}
