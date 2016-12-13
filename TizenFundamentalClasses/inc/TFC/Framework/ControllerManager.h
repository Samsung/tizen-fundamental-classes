/*
 * ControllerManager.h
 *
 *  Created on: Dec 13, 2016
 *      Author: gilang
 */

#ifndef TFC_FRAMEWORK_CONTROLLERMANAGER_H_
#define TFC_FRAMEWORK_CONTROLLERMANAGER_H_

#include "TFC/Core.h"

namespace TFC {
namespace Framework {

class LIBAPI StackingControllerManager: public ControllerManager
{
private:
	std::deque<std::unique_ptr<ControllerBase>> controllerStack;

	Evas_Object* viewContainer;

	void PushController(ControllerBase* controller);
	bool PopController();

	void PushView(ViewBase* view);
	void PopView();

	void DoNavigateBackward();
	void DoNavigateForward(char const* targetControllerName, ObjectClass* data, bool noTrail);
protected:
	virtual ControllerBase& GetCurrentController() const override;
	virtual void PerformNavigation(char const* controllerName, ObjectClass* data, NavigationFlag mode) override;
	virtual Evas_Object* CreateViewContainer(Evas_Object* parent) override;
public:
	/**
	 * Constructor of StackingControllerManager
	 *
	 * @param app IAttachable which this controller manager will attach the underlying view
	 */
	StackingControllerManager();
};

class SwitchingControllerManager: public ControllerManager
{
private:
	ControllerBase* GetController(char const* controllerName);
	ControllerBase* currentController;
protected:
	virtual ControllerBase& GetCurrentController() const override;
public:
	SwitchingControllerManager();
	//void SwitchTo(char const* controllerName);
};



}}



#endif /* TFC_FRAMEWORK_CONTROLLERMANAGER_H_ */
