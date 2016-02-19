/*
 * Core.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: SRIN
 */



#include "SRIN/Core.h"

LIBAPI Event::Event() :
		instance(nullptr), eventHandler(nullptr), eventSource(nullptr), eventLabel(nullptr)
{
}

LIBAPI Event::Event(EventClass* eventSource) :
		instance(nullptr), eventHandler(nullptr), eventSource(eventSource), eventLabel(nullptr)
	{ }

LIBAPI Event::Event(EventClass* instance, EventHandler eventHandler, CString eventLabel) :
		instance(instance), eventHandler(eventHandler), eventSource(nullptr), eventLabel(eventLabel)
	{ }

LIBAPI void Event::operator+=(const Event& other)
{
	this->instance = other.instance;
	this->eventHandler = other.eventHandler;
}

LIBAPI void Event::Invoke(void* eventInfo)
{
	if(this->instance && this->eventHandler)
		(this->instance->*(this->eventHandler))(this, nullptr, eventInfo);
}

LIBAPI void Event::operator()(void* objSource, void* eventData)
{
	if(this->instance && this->eventHandler)
		(this->instance->*(this->eventHandler))(this, objSource, eventData);
}
