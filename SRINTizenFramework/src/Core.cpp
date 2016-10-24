/*
 * Core.cpp
 *
 *  Created on: Mar 22, 2016
 *      Contributor:
 *        Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Core.h"
#include <pthread.h>


LIBAPI PropertyClass::PropertyBase::PropertyBase(void* instance)
{
	// Get current instance from thread specific storage
	this->instance = instance;
}

LIBAPI ObjectClass::~ObjectClass()
{

}

class SomeComponents : public TFC::EventClass, TFC::EventEmitterClass<SomeComponents>
{
private:
	Event<int> eventSomeEvent;
	void AnEventHandler(decltype(eventSomeEvent)* ev, SomeComponents* a, int b)
	{

	}
public:
	SomeComponents()
	{
		eventSomeEvent += { this, &SomeComponents::AnEventHandler };
	}
};
