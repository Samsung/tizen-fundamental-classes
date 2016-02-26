/*
 * Core.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: SRIN
 */

template<class ObjectSourceType, class EventDataType>
inline Event<ObjectSourceType, EventDataType>::Event() :
		instance(nullptr), eventHandler(nullptr), eventSource(nullptr), eventLabel(nullptr)
{
}

template<class ObjectSourceType, class EventDataType>
inline Event<ObjectSourceType, EventDataType>::Event(EventClass* eventSource) :
		instance(nullptr), eventHandler(nullptr), eventSource(eventSource), eventLabel(nullptr)
	{ }

template<class ObjectSourceType, class EventDataType>
inline Event<ObjectSourceType, EventDataType>::Event(EventClass* instance, EventHandler eventHandler, CString eventLabel) :
		instance(instance), eventHandler(eventHandler), eventSource(nullptr), eventLabel(eventLabel)
	{ }

template<class ObjectSourceType, class EventDataType>
template<class EventClassType>
inline Event<ObjectSourceType, EventDataType>::Event(EventClassType* instance, void (EventClassType::* eventHandler)(Event<ObjectSourceType, EventDataType>* eventSource, ObjectSourceType objSource, EventDataType eventData) , CString eventLabel) :
	instance(instance), eventHandler(reinterpret_cast<Event::EventHandler>(eventHandler)), eventSource(nullptr), eventLabel(eventLabel)
	{ }


template<class ObjectSourceType, class EventDataType>
inline void Event<ObjectSourceType, EventDataType>::operator+=(const Event& other)
{
	this->instance = other.instance;
	this->eventHandler = other.eventHandler;
}

template<class ObjectSourceType, class EventDataType>
inline void Event<ObjectSourceType, EventDataType>::Invoke(void* eventInfo)
{
	if(this->instance && this->eventHandler)
		(this->instance->*(this->eventHandler))(this, nullptr, eventInfo);
}

template<class ObjectSourceType, class EventDataType>
inline void Event<ObjectSourceType, EventDataType>::operator()(ObjectSourceType objSource, EventDataType eventData)
{
	if(this->instance && this->eventHandler)
		(this->instance->*(this->eventHandler))(this, objSource, eventData);
}
