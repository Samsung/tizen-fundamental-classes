/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Core.cpp
 *
 * Created on:  Mar 22, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Core.h"
#include "TFC/Core/Reflection.h"

#include <pthread.h>
#include <sstream>
#include <typeinfo>

#include <dlog.h>
#include <execinfo.h>
#include <cxxabi.h>

#include <chrono>

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

#define EXCEPTION_BT_BUFFERSIZE 100


LIBAPI
TFC::TFCException::TFCException(char const* message) {
	this->msg = message;
	BuildStackTrace();
}



void TFC::TFCException::BuildStackTrace()
{
	size_t allocSize = EXCEPTION_BT_BUFFERSIZE;

Backtrace_Alloc:
	void** buffer = new void*[allocSize];
	auto cnt = backtrace(buffer, allocSize);

	if(cnt == allocSize)
	{
		allocSize *= 2;
		delete[] buffer;
		goto Backtrace_Alloc;
	}

	this->symbolCount = cnt;

	this->symbols.reset(backtrace_symbols(buffer, cnt), [] (char** p) {
        free(p);
    });

	delete[] buffer;
}

LIBAPI
TFC::TFCException::~TFCException()
{

}

LIBAPI
std::string TFC::TFCException::GetStackTrace() const
{
	std::string ret;
	std::stringstream strBuf;

	strBuf << "Exception of type: ";

	int status = 0;
	auto typeIdName = typeid(this).name();
	auto execName = abi::__cxa_demangle(typeIdName, nullptr, nullptr, &status);

	if(status == 0)
		strBuf << execName;
	else
		strBuf << typeIdName;

	if(execName)
		free(execName);

	strBuf << " (" << this->msg << ")\n";
	strBuf << "Call trace:\n";

	if(this->symbols)
	{
		// Local symbols;
		auto symbols = this->symbols.get();

		// https://panthema.net/2008/0901-stacktrace-demangled/
		for(int i = 2; i < symbolCount; i++)
		{
			strBuf << (symbolCount - i) << ": ";

			// Tokenize
			char* beginFunc = nullptr;
			char* beginOffset = nullptr;
			char* endOffset = nullptr;

			for(char* p = symbols[i]; *p; p++)
			{
				if(*p == '(')
					beginFunc = p;
				else if(*p == '+')
					beginOffset = p;
				else if(*p == ')' && beginOffset != nullptr)
					endOffset = p;
			}

			if(beginFunc 		!= nullptr
			   and beginOffset	!= nullptr
			   and endOffset 	!= nullptr)
			{
				*beginFunc++ = '\0';
				*beginOffset++ = '\0';
				*endOffset++ = '\0';

				char* ret = abi::__cxa_demangle(beginFunc, nullptr, nullptr, &status);

				if(status == 0)
				{
					strBuf << symbols[i] << ": " << ret << " +" << beginOffset << endOffset << '\n';
				}
				else
				{
					strBuf << symbols[i] << ": " << beginFunc << " +" << beginOffset << endOffset << '\n';
				}

				if(ret != nullptr)
				{
					free(ret);
				}
			}
			else
			{
				strBuf << symbols[i] << '\n';
			}
		}
	}
	else
	{
		strBuf << "Call trace cannot be processed!\n";
	}

	ret = strBuf.str();
	return ret;
}

LIBAPI
TFC::TFCException::TFCException(std::string&& message) {
	this->msg = message;
	BuildStackTrace();
}

LIBAPI
TFC::TFCException::TFCException(std::string const& message) {
	this->msg = message;
	BuildStackTrace();
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

LIBAPI
TFC::ManagedClass::SharedHandle::SharedHandle() : referenceCount(1), isDestructed(false) {
	dlog_print(DLOG_DEBUG, LOG_TAG, "Shared handle created. Handle: %d", (int)this);
}

LIBAPI
void TFC::ManagedClass::SharedHandle::IncrementReference() {
	++this->referenceCount;
	dlog_print(DLOG_DEBUG, LOG_TAG, "Increment reference. Handle: %d, ref: %d", (int)this, this->referenceCount);
}

LIBAPI
bool TFC::ManagedClass::SharedHandle::DecrementReference() {
	dlog_print(DLOG_DEBUG, LOG_TAG, "Decrement reference. Handle: %d, ref: %d", (int)this, this->referenceCount - 1);
	return --this->referenceCount == 0;
}

LIBAPI
bool TFC::ManagedClass::SharedHandle::NotifyDestruction() {
	this->isDestructed = true;
	dlog_print(DLOG_DEBUG, LOG_TAG, "The owner object is destroyed. Handle: %d", (int)this);
	return --this->referenceCount == 0;
}

LIBAPI
TFC::ManagedClass::SharedHandle::~SharedHandle() {
	dlog_print(DLOG_DEBUG, LOG_TAG, "The shared handle is destroyed. Handle: %d", (int)this);
}

LIBAPI
bool TFC::ManagedClass::SharedHandle::IsDestructed() {
	return this->isDestructed;
}

TFC_DefineTypeInfo(TFC::TFCException) {
	{ TFC::Core::Constructor<TFC::TFCException, std::string>(), "StringDefault" }
};

LIBAPI
long long TFC::GetCurrentTimeMillis()
{
	auto time = std::chrono::system_clock::now();
	auto epoch = time.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
	return millis.count();
}

