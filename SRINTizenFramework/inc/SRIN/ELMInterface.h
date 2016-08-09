/*
 * ELMInterface.h
 *
 *  Created on: Feb 16, 2016
 *      Author: SRIN
 */

#ifndef SRINFW_ELMINTERFACE_H_
#define SRINFW_ELMINTERFACE_H_

#error "SRIN/ELMInterface.h header file is deprecated. Consider using SRIN/EFL.h with its new structures."


#include "SRIN/Core.h"

#include <Elementary.h>

void SmartEventHandler(void* data, Evas_Object* obj, void* eventData);
void ObjectEventHandler(void* data, Evas* evas, Evas_Object* obj, void* eventData);
void SignalEventHandler(void *data, Evas_Object *obj, const char *emission, const char *source);
void ObjectItemSignalEventHandler(void *data, Elm_Object_Item *it, const char *emission, const char *source);

struct EdjeSignalInfo
{
	const char* emission;
	const char* source;
};

typedef Event<Evas_Object*, void*> ElementaryEvent;
typedef Event<Evas_Object*, EdjeSignalInfo> EdjeSignalEvent;
typedef Event<Elm_Object_Item*, EdjeSignalInfo> ObjectItemEdjeSignalEvent;



#endif /* ELMINTERFACE_H_ */
