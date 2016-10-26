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

class EvasSmartEventObject : public Event<Evas_Object*, void*>
{

};

class EvasObjectEventObject : public Event<EvasEventSourceInfo*, void*>
{

};

class EdjeSignalEventObject : public Event<Evas_Object*, EdjeSignalInfo>
{

};


class ObjectItemEdjeSignalEventObject : public Event<Elm_Object_Item*, EdjeSignalInfo>
{

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
