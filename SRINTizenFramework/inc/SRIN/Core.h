/*
 * Core.h
 *
 *  Created on: Feb 12, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef CORE_H_
#define CORE_H_


#include <dlog.h>
#include <functional>

#ifdef LIBBUILD
#define LIBAPI __attribute__((__visibility__("default")))
#define LOG_TAG "SRINFW"
#else
#define LIBAPI
#endif

typedef const char* CString;

template<class DefiningClass, class ValueType, ValueType& (DefiningClass::* GetFunc)(), void (DefiningClass::* SetFunc)(const ValueType&)>
class Property {
private:
	DefiningClass* instance;
public:
	Property(DefiningClass* inst) : instance(inst) { }
	operator ValueType() { return (instance->*GetFunc)(); }
	void operator=(const ValueType& val) { (instance->*SetFunc)(val); }
	ValueType* operator->() const { auto ret = (instance->*GetFunc)(); return &ret; }
};

template<class DefiningClass, class ValueType, ValueType DefiningClass::* LocalVar>
class SimpleProperty {
private:
	DefiningClass* instance;
public:
	SimpleProperty(DefiningClass* inst) : instance(inst) { }
	operator ValueType() { return instance->*LocalVar; }
	void operator=(const ValueType& val) { instance->*LocalVar = (val); }
};

template<class DefiningClass, class ValueType, ValueType& (DefiningClass::* GetFunc)()>
class ReadOnlyProperty {
private:
	DefiningClass* instance;
public:
	ReadOnlyProperty(DefiningClass* inst) : instance(inst) { }
	operator ValueType() { return (instance->*GetFunc)(); }
	ValueType* operator->() const { auto ret = (instance->*GetFunc)(); return &ret; }
};

template<class DefiningClass, class ValueType>
class SimpleReadOnlyPropertyBase {
protected:
	ValueType value;
public:
	friend DefiningClass;
	operator ValueType() { return value; }
	ValueType* operator->() const;
};

template<class DefiningClass, class ValueType>
class SimpleReadOnlyProperty : public SimpleReadOnlyPropertyBase<DefiningClass, ValueType> {
private:
	void operator=(const ValueType& val) { this->value = (val); }
public:
	friend DefiningClass;
	ValueType* operator->() const;
};

template<class DefiningClass, class ValueType>
ValueType* SimpleReadOnlyProperty<DefiningClass, ValueType>::operator->() const {
	return &this->value;
}

template<class DefiningClass, class ValueType>
class SimpleReadOnlyProperty<DefiningClass, ValueType*> : public SimpleReadOnlyPropertyBase<DefiningClass, ValueType*> {
private:
	void operator=(ValueType* val) { this->value = (val); }
public:
	friend DefiningClass;
	ValueType* operator->() const;
};

template<class DefiningClass, class ValueType>
ValueType* SimpleReadOnlyProperty<DefiningClass, ValueType*>::operator->() const {
	return this->value;
}

class Event;

class EventClass {};
typedef void (EventClass::*EventHandler)(Event* eventSource, void* objSource, void* eventData);

class Event {
public:
	EventClass* instance;
	EventHandler eventHandler;
	EventClass* eventSource;
	CString eventLabel;

	Event();
	Event(EventClass* eventSource);
	Event(EventClass* instance, EventHandler eventHandler, CString eventLabel = nullptr);

	void operator+=(const Event& other);
	void Invoke(void* eventInfo);
	void operator()(void* objSource, void* eventData);
};

#define AddEventHandler(EVENT_METHOD) ::Event(this, (::EventHandler) & EVENT_METHOD)


#endif /* CORE_H_ */
