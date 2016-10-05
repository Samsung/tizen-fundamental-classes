/*
 * EFL.h
 * Replacement for ELMInterface.h, which applies a new namespace format
 *
 *  Created on: Aug 9, 2016
 *  Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_EFL_H_
#define SRIN_EFL_H_

#include "SRIN/Core.h"
#include <Elementary.h>

namespace SRIN
{
namespace EFL
{
	struct EdjeSignalInfo
	{
		const char* emission;
		const char* source;
	};

	typedef Event<> EcoreJobEvent;

	struct EvasEventSourceInfo
	{
		Evas* evas;
		Evas_Object* obj;
	};

	typedef Event<Evas_Object*, void*> EvasSmartEvent;
	typedef Event<EvasEventSourceInfo*, void*> EvasObjectEvent;
	typedef Event<Evas_Object*, EdjeSignalInfo> EdjeSignalEvent;
	typedef Event<Elm_Object_Item*, EdjeSignalInfo> ObjectItemEdjeSignalEvent;

	void EvasSmartEventHandler(void* data, Evas_Object* obj, void* eventData);
	void EvasObjectEventHandler(void* data, Evas* evas, Evas_Object* obj, void* eventData);
	void EdjeSignalEventHandler(void* data, Evas_Object* obj, const char* emission, const char* source);
	void EdjeObjectItemSignalEventHandler(void* data, Elm_Object_Item* it, const char* emission, const char* source);
d
	void EcoreJobEventHandler(void* d);

	Ecore_Job* QueueJob(EcoreJobEvent& event);
}
}



#endif /* EFL_H_ */
