/*
 * Core.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: SRIN
 */

template<class ObjectSourceType, class EventDataType>
inline Event<ObjectSourceType, EventDataType>::Event() :
	first(nullptr), logDelete(false)
{
}

template<class ObjectSourceType, class EventDataType>
inline Event<ObjectSourceType, EventDataType>::Event(bool logDelete) :
	first(nullptr), logDelete(logDelete)
{
	if(logDelete)
		dlog_print(DLOG_DEBUG, "SRINFW-Event", "Event created %d", this);
}

template<class ObjectSourceType, class EventDataType>
inline Event<ObjectSourceType, EventDataType>::~Event()
{
	if(logDelete)
		dlog_print(DLOG_DEBUG, "SRINFW-Event", "Event deleted %d", this);
	/*
	auto current = this->first;
		
	while(current != nullptr)
	{
		auto deleted = current;
		current = current->next;
		delete deleted;
	}
	*/
}


template<class ObjectSourceType, class EventDataType>
template<class EventClassType>
inline Event<ObjectSourceType, EventDataType>::EventDelegate::EventDelegate(EventClassType* instance, void (EventClassType::* eventHandler)(Event<ObjectSourceType, EventDataType>* eventSource, ObjectSourceType objSource, EventDataType eventData)) :
	instance(instance), eventHandler(reinterpret_cast<Event::EventHandler>(eventHandler))
	{ }


template<class ObjectSourceType, class EventDataType>
inline void Event<ObjectSourceType, EventDataType>::operator+=(const EventDelegate& other)
{
	auto newNode = new EventNode({ other.instance, other.eventHandler, this->first });
	this->first = newNode;
}

template<class ObjectSourceType, class EventDataType>
inline void Event<ObjectSourceType, EventDataType>::operator-=(const EventDelegate& other)
{
	auto current = this->first;
		
	while(current != nullptr)
	{
		if(current->instance == other.instance && current->eventHandler == other.eventHandler)
		{
			//dlog_print(DLOG_DEBUG, "SRINFW-Event", "Deleting %d %d", current->instance, current->eventHandler);
			if(current == this->first)
				this->first = current->next;
			
			auto deleted = current;
			current = current->next;
			delete deleted;
		}
		else
		{
			current = current->next;
		}
	}
}



template<class ObjectSourceType, class EventDataType>
inline void Event<ObjectSourceType, EventDataType>::operator()(ObjectSourceType objSource, EventDataType eventData) const
{
	if(logDelete)
		dlog_print(DLOG_DEBUG, "SRINFW-Event", "Event raise %d", this);
	
	auto current = this->first;
	
	while(current != nullptr)
	{
		if(current->instance && current->eventHandler)
			(current->instance->*(current->eventHandler))(this, objSource, eventData);
		current = current->next;
	}
}
