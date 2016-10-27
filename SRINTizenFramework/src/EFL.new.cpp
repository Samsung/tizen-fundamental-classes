/*
 * EFL.new.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: Gilang M. Hamidy
 */

#include "TFC/EFL.new.h"

void TFC::EFL::EvasSmartEventObject::Bind(Evas_Object* obj,const char* eventName)
{
	evas_object_smart_callback_add(obj, eventName, Callback, this);
	bindingList.AddBinding(obj, eventName);
}

void TFC::EFL::EvasSmartEventObject::Unbind(Evas_Object* obj, const char* eventName)
{

}

void TFC::EFL::EvasSmartEventObject::Callback(void* data,Evas_Object* obj, void* eventInfo)
{
	(*reinterpret_cast<EvasSmartEventObject*>(data))(obj, eventInfo);
}
void TFC::EFL::EvasSmartEventObject::Finalize(Evas_Object* obj, const std::string& eventName, void* data)
{
	evas_object_smart_callback_del_full(obj, eventName.c_str(), Callback, data);
}

TFC::EFL::EvasSmartEventObject::EvasSmartEventObject() : bindingList(this)
{

}
