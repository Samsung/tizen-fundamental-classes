/*
 * Core.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Core.h"
#include <pthread.h>


LIBAPI PropertyClass::PropertyBase::PropertyBase(void* instance)
{
	// Get current instance from thread specific storage
	this->instance = instance;
}

LIBAPI ObjectClass::~ObjectClass()
{

}
