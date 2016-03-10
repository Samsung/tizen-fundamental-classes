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

template<class DefiningClass, class ValueType, ValueType& (DefiningClass::*GetFunc)(), void (DefiningClass::*SetFunc)(
	const ValueType&)>
class Property
{
private:
	DefiningClass* instance;
public:
	Property(DefiningClass* inst) :
		instance(inst)
	{
	}
	operator ValueType()
	{
		return (instance->*GetFunc)();
	}
	void operator=(const ValueType& val)
	{
		(instance->*SetFunc)(val);
	}
	ValueType* operator->() const
	{
		auto ret = (instance->*GetFunc)();
		return &ret;
	}
};

template<class DefiningClass, class ValueType, ValueType DefiningClass::* LocalVar>
class SimpleProperty
{
private:
	DefiningClass* instance;
public:
	SimpleProperty(DefiningClass* inst) :
		instance(inst)
	{
	}
	operator ValueType()
	{
		return instance->*LocalVar;
	}
	void operator=(const ValueType& val)
	{
		instance->*LocalVar = (val);
	}
};

template<class DefiningClass, class ValueType, ValueType (DefiningClass::*GetFunc)()>
class ReadOnlyProperty
{
private:
	DefiningClass* instance;
public:
	ReadOnlyProperty(DefiningClass* inst) :
		instance(inst)
	{
	}
	operator ValueType()
	{
		auto val = (instance->*GetFunc)();
		return val;
	}
};

template<class DefiningClass, class ValueType>
class SimpleReadOnlyPropertyBase
{
protected:
	ValueType value;
public:
	friend DefiningClass;
	operator ValueType()
	{
		return value;
	}
	ValueType* operator->() const;
};

template<class DefiningClass, class ValueType>
class SimpleReadOnlyProperty: public SimpleReadOnlyPropertyBase<DefiningClass, ValueType>
{
private:
	void operator=(const ValueType& val)
	{
		this->value = (val);
	}
public:
	friend DefiningClass;
	const ValueType* operator->() const;
	operator ValueType()
	{
		return this->value;
	}
};

template<class DefiningClass, class ValueType>
const ValueType* SimpleReadOnlyProperty<DefiningClass, ValueType>::operator->() const
{
	return &this->value;
}

template<class DefiningClass, class ValueType>
class SimpleReadOnlyProperty<DefiningClass, ValueType*> : public SimpleReadOnlyPropertyBase<DefiningClass, ValueType*>
{
private:
	void operator=(ValueType* val)
	{
		this->value = (val);
	}
public:
	friend DefiningClass;
	ValueType* operator->() const;
};

template<class DefiningClass, class ValueType>
ValueType* SimpleReadOnlyProperty<DefiningClass, ValueType*>::operator->() const
{
	return this->value;
}

class EventClass
{
};

template<class ObjectSourceType = void*, class EventDataType = void*>
class Event
{
public:
	typedef void (EventClass::*EventHandler)(const Event<ObjectSourceType, EventDataType>* eventSource,
		ObjectSourceType objSource, EventDataType eventData);

	class EventDelegate
	{
		EventClass* instance;
		EventHandler eventHandler;
	public:
		template<class EventClassType>
		EventDelegate(EventClassType* instance,
				void (EventClassType::*eventHandler)(Event<ObjectSourceType, EventDataType>* eventSource,
					ObjectSourceType objSource, EventDataType eventData));

		friend class Event;
	};

	Event();

	void operator+=(const EventDelegate& other);
	void operator()(ObjectSourceType objSource, EventDataType eventData) const;

private:
	struct EventNode
	{
		EventClass* instance;
		EventHandler eventHandler;
		EventNode* next;
	};
	EventNode* first;
};

#include "SRIN/Core.inc"

#define AddEventHandler(EVENT_METHOD) { this, & EVENT_METHOD }

#endif /* CORE_H_ */
