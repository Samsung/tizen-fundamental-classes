/*
 * MVCApplication.h
 *
 *  Created on: Dec 13, 2016
 *      Author: gilang
 */

#ifndef TFC_FRAMEWORK_MVCAPPLICATION_H_
#define TFC_FRAMEWORK_MVCAPPLICATION_H_

#include "TFC/Framework/Application.h"
#include "TFC/Framework/ControllerManager.h"

namespace TFC {
namespace Framework {

class LIBAPI MVCApplicationBase:
	public UIApplicationBase,
	public StackingControllerManager
{
private:
	std::string mainController;
public:
	MVCApplicationBase(char const* appPackage);
	MVCApplicationBase(char const* appPackage, char const* mainController);
	virtual bool OnBackButtonPressed() final;
	virtual void OnApplicationCreated();
	virtual ~MVCApplicationBase();
};

}}



#endif /* TFC_FRAMEWORK_MVCAPPLICATION_H_ */
