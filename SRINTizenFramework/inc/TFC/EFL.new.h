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

template<typename TInfo, typename TCallback, TCallback funcCb, void(*funcFinalize)(Evas_Object*, TInfo const&, void*)>
class BindingList
{
public:
	typedef void(*FinalizeHandler)(Evas_Object*, TInfo const&);

	BindingList(void* yourself);
	~BindingList();
	void AddBinding(Evas_Object* evas, TInfo&& info);
	void RemoveBinding();

private:
	TInfo info;
	void* dataId;

	static void FinalizeCallback(void* data, Evas* es, Evas_Object* obj, void* eventInfo);
};

class EvasSmartEventObject : Core::EventObject<Evas_Object*, void*>
{
public:
	typedef Core::EventObject<Evas_Object*, void*> Type;

	EvasSmartEventObject();

	void Bind(Evas_Object* obj, char const* eventName);
	void Unbind(Evas_Object* obj, char const* eventName);


	using Core::EventObject<Evas_Object*, void*>::operator +=;
	using Core::EventObject<Evas_Object*, void*>::operator -=;
private:

	using Core::EventObject<Evas_Object*, void*>::operator ();
	static void Callback(void* data, Evas_Object* obj, void* eventInfo);
	static void Finalize(Evas_Object* obj, std::string const& eventName, void* data);

	BindingList<std::string, Evas_Smart_Cb, Callback, Finalize> bindingList;

};

class EvasObjectEventObject : Core::EventObject<EvasEventSourceInfo*, void*>
{
public:
	void Bind(Evas_Object* obj, Evas_Callback_Type eventType);

private:
	static void Callback(void* data, Evas* es, Evas_Object* obj, void* eventInfo);

};

class EdjeSignalEventObject : Core::EventObject<Evas_Object*, EdjeSignalInfo>
{
public:
	void Bind(Evas_Object* obj, char const* emission, char const* source);

private:
	static void Callback(void* data, Evas_Object* obj, char const* emission, char const* source);
};

class ObjectItemEdjeSignalEventObject : Core::EventObject<Elm_Object_Item*, EdjeSignalInfo>
{
public:
	void Bind(Elm_Object_Item*, const char *emission, const char *source);

private:
	static void Callback(void* data, Elm_Object_Item* it, const char* emission, const char* source);
};

class EFLProxyClass : public TFC::EventClass
{
protected:
	using EvasSmartEvent 			= EvasSmartEventObject;
	using EvasObjectEvent 			= EvasObjectEventObject;
	using EdjeSignalEvent 			= EdjeSignalEventObject;
	using ObjectItemEdjeSignalEvent = ObjectItemEdjeSignalEventObject;
};

}
}

template<typename TInfo, typename TCallback, TCallback funcCb,
		 void (*funcFinalize)(Evas_Object*, const TInfo&, void*)>
TFC::EFL::BindingList<TInfo, TCallback, funcCb, funcFinalize>::
BindingList(void* yourself) :
	dataId(yourself)
{
}

template<typename TInfo, typename TCallback, TCallback funcCb,
		 void (*funcFinalize)(Evas_Object*, const TInfo&, void*)>
TFC::EFL::BindingList<TInfo, TCallback, funcCb, funcFinalize>::
~BindingList()
{
}

template<typename TInfo, typename TCallback, TCallback funcCb,
		 void (*funcFinalize)(Evas_Object*, const TInfo&, void*)>
void TFC::EFL::BindingList<TInfo, TCallback, funcCb, funcFinalize>::
	 AddBinding(Evas_Object* evas, TInfo&& info)
{

}

template<typename TInfo, typename TCallback, TCallback funcCb,
		 void (*funcFinalize)(Evas_Object*, const TInfo&, void*)>
void TFC::EFL::BindingList<TInfo, TCallback, funcCb, funcFinalize>::
	 RemoveBinding()
{

}


template<typename TInfo, typename TCallback, TCallback funcCb,
		void (*funcFinalize)(Evas_Object*, const TInfo&, void*)>
void TFC::EFL::BindingList<TInfo, TCallback, funcCb, funcFinalize>::
	 FinalizeCallback(void* data, Evas* es, Evas_Object* obj, void* eventInfo)
{
	auto o = reinterpret_cast<TFC::EFL::BindingList<TInfo, TCallback, funcCb, funcFinalize>*>(data);

}

#endif /* EFL_NEW_H_ */
