/*
 * Core.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: SRIN
 */



#include "SRIN/Core.h"

Event::Event() :
		instance(nullptr), eventHandler(nullptr), eventSource(nullptr), eventLabel(nullptr)
{
}

Event::Event(EventClass* eventSource) :
		instance(nullptr), eventHandler(nullptr), eventSource(eventSource), eventLabel(nullptr)
	{ }

Event::Event(EventClass* instance, EventHandler eventHandler, CString eventLabel = nullptr) :
		instance(instance), eventHandler(eventHandler), eventSource(nullptr), eventLabel(eventLabel)
	{ }

void Event::operator=(const Event& other)
{
	this->instance = other.instance;
	this->eventHandler = other.eventHandler;
}

void Event::Invoke(void* eventInfo)
{
	if(this->instance && this->eventHandler)
		(this->instance->*(this->eventHandler))(this, nullptr, eventInfo);
}

void Event::operator()(Evas_Object* objSource, void* eventData)
{
	if(this->instance && this->eventHandler)
		(this->instance->*(this->eventHandler))(this, objSource, eventData);
}
