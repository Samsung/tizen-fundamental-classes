/*
 * ELMInterface.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: SRIN
 */

#include "SRIN/ELMInterface.h"


void SmartEventHandler(void* data, Evas_Object* obj, void* eventData)
{
	Event<>* package = static_cast<Event<>*>(data);

#if _DEBUG
	if(package->eventLabel)
		dlog_print(DLOG_DEBUG, LOG_TAG, "Event raised: %s", package->eventLabel);
#endif

	(*package)(obj, eventData);
}

void ObjectEventHandler(void* data, Evas* evas, Evas_Object* obj, void* eventData)
{
	Event<>* package = static_cast<Event<>*>(data);

#if _DEBUG
	if(package->eventLabel)
		dlog_print(DLOG_DEBUG, LOG_TAG, "Event raised: %s", package->eventLabel);
#endif

	(*package)(obj, eventData);
}

