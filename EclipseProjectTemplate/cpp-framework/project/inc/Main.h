/*
 * $(appName).h
 *
 *  Created on: Apr 11, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef __$(appName)_H_
#define __$(appName)_H_

#include "SRIN/Framework/Application.h"

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "$(appName)"

#if !defined(PACKAGE)
#define PACKAGE "$(packageName)"
#endif


// Define controller name here. This is to identify controller without having to load the entire class header
#define CONTROLLER_SPLASHSCREEN		"SplashScreenController"

class $(appName)AppClass : public SRIN::Framework::MVCApplicationBase
{
public:
	$(appName)AppClass();
	virtual void OnApplicationCreated();
};

extern $(appName)AppClass $(appName)App;

#endif /* $(appName)_H_ */
