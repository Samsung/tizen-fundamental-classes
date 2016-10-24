/*
 * ELMInterface.h
 *
 *  Created on: Feb 16, 2016
 *      Contributor:
 *        Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef TFCFW_ELMINTERFACE_H_
#define TFCFW_ELMINTERFACE_H_

#ifndef USE_LEGACY_EFL_INTERFACE
#error "TFC/ELMInterface.h header file is deprecated. Consider using TFC/EFL.h with its new structures."
#else
#warning "TFC/ELMInterface.h header file is deprecated. Consider using TFC/EFL.h with its new structures"

#include "TFC/Core.h"
#include "TFC/EFL.h"

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

typedef TFC::EFL::EvasSmartEvent ElementaryEvent;
typedef Event<Evas_Object*, EdjeSignalInfo> EdjeSignalEvent;
typedef Event<Elm_Object_Item*, EdjeSignalInfo> ObjectItemEdjeSignalEvent;

#endif

#endif /* ELMINTERFACE_H_ */
