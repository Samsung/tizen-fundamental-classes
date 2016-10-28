/*
 * Core.cpp
 *
 *  Created on: Mar 22, 2016
 *      Contributor:
 *        Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Core.h"
#include "TFC/Core.new.h"
#include <pthread.h>



LIBAPI PropertyClass::PropertyBase::PropertyBase(void* instance)
{
	// Get current instance from thread specific storage
	this->instance = instance;
}

LIBAPI ObjectClass::~ObjectClass()
{

}

LIBAPI TFC::Core::PropertyObjectBase::PropertyObjectBase(void* instance) : instance(instance)
{

}

LIBAPI TFC::ObjectClass::~ObjectClass()
{

}

LIBAPI
TFC::TFCException::TFCException(char const* message) {
	this->msg = message;
}

LIBAPI
TFC::TFCException::TFCException(std::string&& message) {
	this->msg = message;
}

LIBAPI
TFC::TFCException::TFCException(std::string const& message) {
	this->msg = message;
}

LIBAPI
char const* TFC::TFCException::what() const throw () {
	return this->msg.c_str();
}
