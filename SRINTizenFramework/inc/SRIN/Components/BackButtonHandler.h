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
	BackButtonHandler();
	void Acquire();
	void Release();
	virtual bool BackButtonClicked() = 0;
public:




	virtual ~BackButtonHandler();
};

}
}


#endif /* SRIN_COMPONENTS_BACKBUTTONHANDLER_H_ */
