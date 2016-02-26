/*
 * ELMInterface.h
 *
 *  Created on: Feb 16, 2016
 *      Author: SRIN
 */

#ifndef ELMINTERFACE_H_
#define ELMINTERFACE_H_

#include "SRIN/Core.h"

#include <Elementary.h>

void SmartEventHandler(void* data, Evas_Object* obj, void* eventData);
void ObjectEventHandler(void* data, Evas* evas, Evas_Object* obj, void* eventData);

typedef Event<Evas_Object*, void*> ElementaryEvent;

#endif /* ELMINTERFACE_H_ */
