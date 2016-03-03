/*
 * ELMInterface.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: SRIN
 */

#include "SRIN/ELMInterface.h"


LIBAPI void SmartEventHandler(void* data, Evas_Object* obj, void* eventData)
{
	auto package = static_cast<Event<Evas_Object*, void*>*>(data);

#if _DEBUG
	if(package->eventLabel)
		dlog_print(DLOG_DEBUG, LOG_TAG, "Event raised: %s", package->eventLabel);
#endif

	(*package)(obj, eventData);
}

LIBAPI void ObjectEventHandler(void* data, Evas* evas, Evas_Object* obj, void* eventData)
{
	auto package = static_cast<Event<Evas_Object*, void*>*>(data);

#if _DEBUG
	if(package->eventLabel)
		dlog_print(DLOG_DEBUG, LOG_TAG, "Event raised: %s", package->eventLabel);
#endif

	(*package)(obj, eventData);
}

