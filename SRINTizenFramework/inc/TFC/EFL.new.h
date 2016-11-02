/*
 * EFL.new.h
 *
 *  Created on: Oct 26, 2016
 *      Author: Gilang M. Hamidy
 */
#pragma once

#ifndef EFL_NEW_H_
#define EFL_NEW_H_

#include "TFC/Core.new.h"
#include <Elementary.h>

namespace TFC {
namespace EFL {

EXCEPTION_DECLARE		(EFLException, TFCException);
EXCEPTION_DECLARE_MSG	(EventBoundException, EFLException, "This event object is already bound");

struct EdjeSignalInfo
{
	const char* emission;
	const char* source;
};

struct EvasEventSourceInfo
{
	Evas* evas;
	Evas_Object* obj;
};

template<typename TEventData = void>
class EvasSmartEventObjectBase : Core::EventObject<Evas_Object*, TEventData*>
{
public:
	typedef Core::EventObject<Evas_Object*, TEventData*> Type;

	EvasSmartEventObjectBase();
	~EvasSmartEventObjectBase();

	void Bind(Evas_Object* obj, char const* eventName);
	void Unbind();


	using Core::EventObject<Evas_Object*, TEventData*>::operator +=;
	using Core::EventObject<Evas_Object*, TEventData*>::operator -=;
private:
	using Core::EventObject<Evas_Object*, TEventData*>::operator ();

	static void Callback(void* data, Evas_Object* obj, void* eventInfo);
	static void Finalize(void* data, Evas* e, Evas_Object* obj, void* event_info);

	char const* eventName;
	Evas_Object* boundObject;
};

typedef EvasSmartEventObjectBase<> EvasSmartEventObject;

class EvasObjectEventObject : Core::EventObject<EvasEventSourceInfo, void*>
{
public:
	typedef Core::EventObject<EvasEventSourceInfo, void*> Type;

	EvasObjectEventObject();
	~EvasObjectEventObject();

	void Bind(Evas_Object* obj, Evas_Callback_Type eventType);
	void Unbind();

	using Core::EventObject<EvasEventSourceInfo, void*>::operator +=;
	using Core::EventObject<EvasEventSourceInfo, void*>::operator -=;
private:
	static void Callback(void* data, Evas* es, Evas_Object* obj, void* eventInfo);
	static void Finalize(void* data, Evas* es, Evas_Object* obj, void* eventInfo);

	Evas_Callback_Type eventType;
	Evas_Object* boundObject;
};

class EdjeSignalEventObject : Core::EventObject<Evas_Object*, EdjeSignalInfo>
{
public:
	typedef Core::EventObject<Evas_Object*, EdjeSignalInfo> Type;

	EdjeSignalEventObject();
	~EdjeSignalEventObject();

	void Bind(Evas_Object* obj, char const* emission, char const* source);
	void Unbind();

	using Core::EventObject<Evas_Object*, EdjeSignalInfo>::operator +=;
	using Core::EventObject<Evas_Object*, EdjeSignalInfo>::operator -=;
private:
	static void Callback(void* data, Evas_Object* obj, char const* emission, char const* source);
	static void Finalize(void* data, Evas* es, Evas_Object* obj, void* eventInfo);

	Evas_Object* boundObject;
	char const* emission;
	char const* source;
};

class ObjectItemEdjeSignalEventObject : Core::EventObject<Elm_Object_Item*, EdjeSignalInfo>
{
public:
	typedef Core::EventObject<Elm_Object_Item*, EdjeSignalInfo> Type;

	ObjectItemEdjeSignalEventObject();
	~ObjectItemEdjeSignalEventObject();

	void Bind(Elm_Object_Item* objIt, const char *emission, const char *source);
	void Unbind();

	using Core::EventObject<Elm_Object_Item*, EdjeSignalInfo>::operator +=;
	using Core::EventObject<Elm_Object_Item*, EdjeSignalInfo>::operator -=;
private:
	static void Callback(void* data, Elm_Object_Item* it, const char* emission, const char* source);
	static void Finalize(void* data, Evas* es, Evas_Object* obj, void* eventInfo);

	Elm_Object_Item* boundObject;
	char const* emission;
	char const* source;
};

/**
 * EFLProxyClass is an attribute class which introduces EFL proxy objects in the subclass of this
 * class. EFL proxy object is used to delegate event which happens on EFL infrastructure into C++
 * class system automatically. This enables EFL event handling to be written in object-oriented
 * manner.
 *
 * EFL uses Flat-C API pattern which makes it uses procedural approach in calling callback for
 * handling event. The API user cannot pass C++ non-static function thus disabling the object-
 * oriented infrastructure when using EFL event functionality directly. EFL proxy objects overcome
 * this limitation by wrapping EFL event with TFC's Core Event Handling infrastructure.
 */
class EFLProxyClass : public virtual TFC::EventClass
{
protected:
	using EvasSmartEvent 			= EvasSmartEventObject;
	using EvasObjectEvent 			= EvasObjectEventObject;
	using EdjeSignalEvent 			= EdjeSignalEventObject;
	using ObjectItemEdjeSignalEvent = ObjectItemEdjeSignalEventObject;

	template<typename T>
	void InvokeLater(void (T::*func)(void));
};

}
}

template<typename T>
void TFC::EFL::EFLProxyClass::InvokeLater(void (T::*func)(void))
{
	struct payload {
		T* ptr;
		void (T::*func)(void);
	}* p = new payload({ static_cast<T*>(this), func });
	ecore_job_add([] (void* data) {
		auto p 		= reinterpret_cast<payload*>(data);
		auto func 	= p->func;
		auto ptr 	= p->ptr;
		(ptr->*func)();
		delete p;
	}, p);
}

template<typename T>
void TFC::EFL::EvasSmartEventObjectBase<T>::Callback(void* data,Evas_Object* obj, void* eventInfo)
{
	(*reinterpret_cast<EvasSmartEventObjectBase<T>*>(data))(obj, reinterpret_cast<T*>(eventInfo));
}

template<typename T>
void TFC::EFL::EvasSmartEventObjectBase<T>::Finalize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	auto d = reinterpret_cast<EvasSmartEventObjectBase<T>*>(data);
	d->boundObject = nullptr;
	d->eventName = nullptr;
}

template<typename T>
TFC::EFL::EvasSmartEventObjectBase<T>::EvasSmartEventObjectBase() : eventName(nullptr), boundObject(nullptr)
{

}

template<typename T>
TFC::EFL::EvasSmartEventObjectBase<T>::~EvasSmartEventObjectBase()
{
	this->Unbind();
}

template<typename T>
void TFC::EFL::EvasSmartEventObjectBase<T>::Bind(Evas_Object* obj,const char* eventName)
{
	if(this->boundObject != nullptr)
		throw EventBoundException();

	evas_object_smart_callback_add(obj, eventName, Callback, this);
	evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, Finalize, this);

	this->boundObject = obj;
	this->eventName = eventName;
}

template<typename T>
void TFC::EFL::EvasSmartEventObjectBase<T>::Unbind()
{
	if(this->boundObject == nullptr)
		return;

	evas_object_smart_callback_del_full(boundObject, eventName, Callback, this);
	evas_object_event_callback_del_full(boundObject, EVAS_CALLBACK_DEL, Finalize, this);

	this->boundObject = nullptr;
	this->eventName = nullptr;
}

#endif /* EFL_NEW_H_ */
