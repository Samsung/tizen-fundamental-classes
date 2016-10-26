#pragma once

#ifndef TFC_CORE_EVENT_INC_H
#define TFC_CORE_EVENT_INC_H

#include <memory>

#ifndef CORE_NEW_H_
#include "TFC/Core.new.h"
#endif


class TFC::EventClass
{

};

template<typename TBase>
class TFC::EventEmitterClass
{
public:
	template<typename TEventData>
	using Event = TFC::Core::EventObject<TBase*, TEventData>;
};

template<typename TObjectSource, typename TEventData>
class TFC::Core::EventObject
{
public:
	typedef void (EventClass::*EventHandler)(const TFC::Core::EventObject<TObjectSource, TEventData>* eventSource,
											 TObjectSource objSource,
											 TEventData eventData);

	class EventDelegate;

	EventObject();
	EventObject(bool logDelete);
	~EventObject();
	void operator+=(const EventDelegate& other);
	void operator-=(const EventDelegate& other);
	void operator()(TObjectSource objSource, TEventData eventData) const;

private:
	struct EventNode;

	EventNode* first;
	bool logDelete;
};

template<typename TObjectSource, typename TEventData>
struct TFC::Core::EventObject<TObjectSource, TEventData>::EventNode
{
	EventClass* instance;
	EventHandler eventHandler;
	EventNode* next;
};

template<typename TObjectSource, typename TEventData>
class TFC::Core::EventObject<TObjectSource, TEventData>::EventDelegate
{
	EventClass* instance;
	EventHandler eventHandler;

	template<typename TEventClass>
	struct EventHandlerFunc {
		typedef void (TEventClass::*Type)(
						TFC::Core::EventObject<TObjectSource, TEventData>* eventSource,
						TObjectSource objSource,
						TEventData eventData);
	};

public:
	template<class TEventClass>
	EventDelegate(TEventClass* instance, typename EventHandlerFunc<TEventClass>::Type eventHandler);
	template<typename, typename>
	friend class TFC::Core::EventObject;
};

template<typename TObjectSource, typename TEventData>
class TFC::Core::SharedEventObject : protected std::shared_ptr<TFC::Core::EventObject<TObjectSource, TEventData>>
{
public:
	SharedEventObject();
	void operator+=(const typename TFC::Core::EventObject<TObjectSource, TEventData>::EventDelegate& other);
	void operator-=(const typename TFC::Core::EventObject<TObjectSource, TEventData>::EventDelegate& other);
	void operator()(TObjectSource objSource, TEventData eventData) const;
};

template<class TObjectSource, class TEventData>
TFC::Core::EventObject<TObjectSource, TEventData>::EventObject() :
	first(nullptr), logDelete(false)
{
}

template<class TObjectSource, class TEventData>
TFC::Core::EventObject<TObjectSource, TEventData>::EventObject(bool logDelete) :
	first(nullptr), logDelete(logDelete)
{
#if VERBOSE
	if(logDelete)
		dlog_print(DLOG_DEBUG, "SRINFW-Event", "Event created %d", this);
#endif
}

template<class TObjectSource, class TEventData>
TFC::Core::EventObject<TObjectSource, TEventData>::~EventObject()
{
#if VERBOSE
	if(logDelete)
		dlog_print(DLOG_DEBUG, "SRINFW-Event", "Event deleted %d", this);
#endif

	auto current = this->first;

	while(current != nullptr)
	{
		auto deleted = current;
		current = current->next;
		delete deleted;
	}

}


template<class TObjectSource, class TEventData>
template<class TEventClass>
TFC::Core::EventObject<TObjectSource, TEventData>::EventDelegate::EventDelegate(
		TEventClass* instance,
		typename  EventHandlerFunc<TEventClass>::Type eventHandler) :
	instance(instance),
	eventHandler(reinterpret_cast<TFC::Core::EventObject<TObjectSource, TEventData>::EventHandler>(eventHandler))
{
}


template<class TObjectSource, class TEventData>
void TFC::Core::EventObject<TObjectSource, TEventData>::operator+=(const EventDelegate& other)
{
	auto newNode = new EventNode({ other.instance, other.eventHandler, this->first });
	this->first = newNode;
}

template<class TObjectSource, class TEventData>
void TFC::Core::EventObject<TObjectSource, TEventData>::operator-=(const EventDelegate& other)
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



template<class TObjectSource, class TEventData>
void TFC::Core::EventObject<TObjectSource, TEventData>::operator() (TObjectSource objSource,
		TEventData eventData) const
{
#if VERBOSE
	if(logDelete)
		dlog_print(DLOG_DEBUG, "SRINFW-Event", "Event raise %d", this);
#endif

	auto current = this->first;

	while(current != nullptr)
	{
		if(current->instance && current->eventHandler)
			(current->instance->*(current->eventHandler))(this, objSource, eventData);
		current = current->next;
	}
}


template<typename TObjectSource, typename TEventData>
TFC::Core::SharedEventObject<TObjectSource, TEventData>::SharedEventObject() :
	std::shared_ptr<TFC::Core::EventObject<TObjectSource, TEventData>>(
			new TFC::Core::EventObject<TObjectSource, TEventData>(true))
{

}

template<typename TObjectSource, typename TEventData>
void TFC::Core::SharedEventObject<TObjectSource, TEventData>::operator+=(
		const typename TFC::Core::EventObject<TObjectSource, TEventData>::EventDelegate& other)
{
	// Forwarding the call to the EventObject stored inside this shared_ptr
	this->get()->operator +=(other);
}

template<typename TObjectSource, typename TEventData>
void TFC::Core::SharedEventObject<TObjectSource, TEventData>::operator-=(
		const typename TFC::Core::EventObject<TObjectSource, TEventData>::EventDelegate& other)
{
	// Same as above
	this->get()->operator -=(other);
}

template<typename TObjectSource, typename TEventData>
void TFC::Core::SharedEventObject<TObjectSource, TEventData>:: operator()(
		TObjectSource objSource, TEventData eventData) const
{
	// Same as above
	this->get()->operator ()(objSource, eventData);
}

#endif
