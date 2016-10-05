/*
 * ELMInterface.cpp
 *
 *  Created on: Feb 16, 2016
 *      Contributor:
 *        Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#define USE_LEGACY_EFL_INTERFACE
#include "SRIN/ELMInterface.h"

LIBAPI void SmartEventHandler(void* data, Evas_Object* obj, void* eventData)
{
	SRIN::EFL::EvasSmartEventHandler(data, obj, eventData);
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

LIBAPI void SignalEventHandler(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	auto package = static_cast<EdjeSignalEvent*>(data);
	(*package)(obj, { emission, source });
}

LIBAPI void ObjectItemSignalEventHandler(void *data, Elm_Object_Item *it, const char *emission, const char *source)
{
	auto package = static_cast<ObjectItemEdjeSignalEvent*>(data);
	(*package)(it, { emission, source });
}
