/*
 * EFL.new.h
 *
 *  Created on: Oct 26, 2016
 *      Author: Gilang M. Hamidy
 */

#ifndef EFL_NEW_H_
#define EFL_NEW_H_

#include "TFC/Core.new.h"
#include <Elementary.h>

namespace TFC {
namespace EFL {

struct EvasEventSourceInfo
{
	Evas* evas;
	Evas_Object* obj;
};

class EvasSmartEventObject : Event<Evas_Object*, void*>
{
private:
	static void Callback(void* data, Evas_Object* obj, void* eventInfo);
public:
	void Bind(Evas_Object* obj, char const* eventName);
	using Event<Evas_Object*, void*>::operator +=;
};

class EvasObjectEventObject : Event<EvasEventSourceInfo*, void*>
{
private:
	static void Callback(void* data, Evas* es, Evas_Object* obj, void* eventInfo);

public:
	void Bind(Evas_Object* obj, Evas_Callback_Type eventType);
};

class EdjeSignalEventObject : Event<Evas_Object*, EdjeSignalInfo>
{
private:
	static void Callback(void* data, Evas_Object* obj, char const* emission, char const* source);

public:
	void Bind(Evas_Object* obj, char const* emission, char const* source);
};

class ObjectItemEdjeSignalEventObject : Event<Elm_Object_Item*, EdjeSignalInfo>
{
private:
	static void Callback(void* data, Elm_Object_Item* it, const char* emission, const char* source);

public:
	void Bind(Elm_Object_Item*, const char *emission, const char *source);
};

class EFLProxyClass
{
protected:
	using EvasSmartEvent 			= EvasSmartEventObject;
	using EvasObjectEvent 			= EvasObjectEventObject;
	using EdjeSignalEvent 			= EdjeSignalEventObject;
	using ObjectItemEdjeSignalEvent = ObjectItemEdjeSignalEventObject;
};

}
}



#endif /* EFL_NEW_H_ */
