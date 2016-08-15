/*
 * BackButtonHandler.h
 *
 *  Created on: Apr 27, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_COMPONENTS_BACKBUTTONHANDLER_H_
#define SRIN_COMPONENTS_BACKBUTTONHANDLER_H_

#include "SRIN/Framework/Application.h"

namespace SRIN {
namespace Components {

/**
 * Class that can be inherited to override back button default behavior.
 * It provides a way to manipulate UIApplicationBase's back button stack with Acquire() & Release() method.
 * When the handler is acquired, every back button press event will call a custom function BackButtonClicked().
 */
class LIBAPI BackButtonHandler : public virtual EventClass
{
private:
	class Wrapper : public EventClass
	{
		BackButtonHandler* handler;
	public:
		Wrapper(BackButtonHandler* handler);
		void Call();
	};

	friend class Wrapper;
	Wrapper* obj;
	bool acquired;
	bool BackButtonClickedInternal();
protected:
	/**
	 * Constructor for BackButtonHandler.
	 */
	BackButtonHandler();

	/**
	 * Method to acquire handler of back button.
	 * It calls AcquireExclusiveBackButtonPressed() method from UIApplicationBase which pushes the handler to the stack.
	 */
	void Acquire();

	/**
	 * Method to release handler of back button.
	 * It calls ReleaseExclusiveBackButtonPressed() method from UIApplicationBase which pops the handler to the stack.
	 */
	void Release();

	/**
	 * Abstract method that will be called when the handler is acquired and back button is pressed.
	 * Has to be implemented.
	 */
	virtual bool BackButtonClicked() = 0;
public:
	/**
	 * Destructor of BackButtonHandler.
	 */
	virtual ~BackButtonHandler();
};

}
}


#endif /* SRIN_COMPONENTS_BACKBUTTONHANDLER_H_ */
