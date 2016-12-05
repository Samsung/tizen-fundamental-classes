/*
 * Core.cpp
 *
 *  Created on: Mar 22, 2016
 *      Contributor:
 *        Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Core.h"
#include <pthread.h>
#include <sstream>
#include <typeinfo>

#include <dlog.h>

class TFC::ManagedClass::SharedHandle
{
public:
	SharedHandle();
	~SharedHandle();
	void IncrementReference();
	bool DecrementReference();
	bool NotifyDestruction();
	bool IsDestructed();
private:
	int referenceCount;
	bool isDestructed;
};

LIBAPI TFC::Core::PropertyObjectBase::PropertyObjectBase(void* instance) : instance(instance)
{

}

LIBAPI TFC::ObjectClass::~ObjectClass()
{

}

LIBAPI
TFC::TFCException::TFCException(char const* message) {
	//this->msg = message;

	std::stringstream strBuf;
	strBuf << "Exception of type: ";
	strBuf << typeid(this).name();
	strBuf << " (" << message << ")\n";
	strBuf << "Call trace:\n";


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

LIBAPI
bool TFC::ManagedClass::SafePointer::TryAccess() const {
	bool val = !(this->handle == nullptr || this->handle->IsDestructed());
	if(val)
		dlog_print(DLOG_DEBUG, LOG_TAG, "Try access the SafePointer: true");
	else
		dlog_print(DLOG_DEBUG, LOG_TAG, "Try access the SafePointer: false");
	return val;
}

LIBAPI
TFC::ManagedClass::SafePointer::SafePointer() : handle(nullptr)
{
}

LIBAPI
TFC::ManagedClass::SafePointer::SafePointer(const SafePointer& that)
{
	this->handle = that.handle;
	this->handle->IncrementReference();

	dlog_print(DLOG_DEBUG, LOG_TAG, "Safe pointer copied: %d, handle: %d", (int)this, (int)this->handle);
}

LIBAPI
TFC::ManagedClass::SafePointer::SafePointer(SharedHandle* handle) : handle(handle) {
	handle->IncrementReference();

	dlog_print(DLOG_DEBUG, LOG_TAG, "Safe pointer created: %d, handle: %d", (int)this, (int)this->handle);
}

LIBAPI
TFC::ManagedClass::SafePointer::SafePointer(SafePointer&& that) {
	this->handle = that.handle;
	that.handle = nullptr;

	dlog_print(DLOG_DEBUG, LOG_TAG, "Safe pointer moved: %d, handle: %d", (int)this, (int)this->handle);
}

LIBAPI
TFC::ManagedClass::SafePointer::~SafePointer() {
	dlog_print(DLOG_DEBUG, LOG_TAG, "Safe pointer deleted: %d, handle: %d", (int)this, (int)this->handle);

	if(this->handle != nullptr && this->handle->DecrementReference())
		delete this->handle;
}

LIBAPI
TFC::ManagedClass::ManagedClass() : handle(nullptr) {
}

LIBAPI
TFC::ManagedClass::~ManagedClass() {
	if(this->handle != nullptr && this->handle->NotifyDestruction())
		delete this->handle;
}

LIBAPI
TFC::ManagedClass::SafePointer TFC::ManagedClass::GetSafePointer()
{
	if(this->handle == nullptr)
		this->handle = new SharedHandle;
	return { this->handle };
}

TFC::ManagedClass::SharedHandle::SharedHandle() : referenceCount(1), isDestructed(false) {
	dlog_print(DLOG_DEBUG, LOG_TAG, "Shared handle created. Handle: %d", (int)this);
}

void TFC::ManagedClass::SharedHandle::IncrementReference() {
	++this->referenceCount;
	dlog_print(DLOG_DEBUG, LOG_TAG, "Increment reference. Handle: %d, ref: %d", (int)this, this->referenceCount);
}

bool TFC::ManagedClass::SharedHandle::DecrementReference() {
	dlog_print(DLOG_DEBUG, LOG_TAG, "Decrement reference. Handle: %d, ref: %d", (int)this, this->referenceCount - 1);
	return --this->referenceCount == 0;
}

bool TFC::ManagedClass::SharedHandle::NotifyDestruction() {
	this->isDestructed = true;
	dlog_print(DLOG_DEBUG, LOG_TAG, "The owner object is destroyed. Handle: %d", (int)this);
	return --this->referenceCount == 0;
}

TFC::ManagedClass::SharedHandle::~SharedHandle() {
	dlog_print(DLOG_DEBUG, LOG_TAG, "The shared handle is destroyed. Handle: %d", (int)this);
}

bool TFC::ManagedClass::SharedHandle::IsDestructed() {
	return this->isDestructed;
}


