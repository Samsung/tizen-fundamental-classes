/*
 * Core.h
 *
 *  Created on: Feb 12, 2016
 *      Author: gilang
 */

#ifndef CORE_H_
#define CORE_H_

typedef const char* CString;
template<class DefiningClass, class ValueType, ValueType (DefiningClass::* GetFunc)(), void (DefiningClass::* SetFunc)(const ValueType&)>
class Property {
private:
	DefiningClass* instance;
public:
	Property(DefiningClass* inst) : instance(inst) { }
	operator ValueType() { return (instance->*GetFunc)(); }
	void operator=(const ValueType& val) { (instance->*SetFunc)(val); }
	ValueType &operator->() const { return instance->*GetFunc(); }
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

template<class DefiningClass, class ValueType>
class ReadOnlyProperty {
private:
	DefiningClass* instance;
	ValueType (DefiningClass::* GetFunc)();
public:
	ReadOnlyProperty(DefiningClass* inst, ValueType (DefiningClass::* getFunc)()) : instance(inst), GetFunc(getFunc) { }
	operator ValueType() { return instance->*GetFunc(); }
};


class Event;

class EventClass {};
typedef void (EventClass::*EventHandler)(Event* eventSource, Evas_Object* objSource, void* eventData);

class Event {
public:
	EventClass* instance;
	EventHandler eventHandler;
	EventClass* eventSource;
	CString eventLabel;

	inline Event() :
		instance(nullptr), eventHandler(nullptr), eventSource(nullptr), eventLabel(nullptr)
	{ }

	inline Event(EventClass* eventSource) :
		instance(nullptr), eventHandler(nullptr), eventSource(eventSource), eventLabel(nullptr)
	{ }

	inline Event(EventClass* instance, EventHandler eventHandler, CString eventLabel = nullptr) :
		instance(instance), eventHandler(eventHandler), eventSource(nullptr), eventLabel(eventLabel)
	{ }

	inline void operator=(const Event& other)
	{
		this->instance = other.instance;
		this->eventHandler = other.eventHandler;
	}

	inline void Invoke(void* eventInfo)
	{
		if(this->instance && this->eventHandler)
			(this->instance->*(this->eventHandler))(this, nullptr, eventInfo);
	}

	inline void operator()(Evas_Object* objSource, void* eventData)
	{
		if(this->instance && this->eventHandler)
			(this->instance->*(this->eventHandler))(this, objSource, eventData);
	}
};

void SmartEventHandler(void* data, Evas_Object* obj, void* eventData);
void ObjectEventHandler(void* data, Evas* evas, Evas_Object* obj, void* eventData);


#define AddEventHandler(EVENT_METHOD) ::Event(this, (::EventHandler) & EVENT_METHOD)


#endif /* CORE_H_ */
