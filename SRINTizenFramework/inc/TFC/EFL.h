/*
 * EFL.h
 * Replacement for ELMInterface.h, which applies a new namespace format
 *
 *  Created on: Aug 9, 2016
 *  Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifdef USE_TFC_CORE
// For migratory purpose
#include "TFC/EFL.new.h"
#else

#ifndef TFC_EFL_H_
#define TFC_EFL_H_

#include "TFC/Core.h"
#include <Elementary.h>

namespace TFC
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

	void EcoreJobEventHandler(void* d);

	Ecore_Job* QueueJob(EcoreJobEvent& event);
}
}



#endif /* EFL_H_ */
#endif
