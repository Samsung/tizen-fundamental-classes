/*
 * Core.new.h
 *
 *  Created on: Oct 25, 2016
 *      Author: Gilang M. Hamidy
 */

#ifndef CORE_NEW_H_
#define CORE_NEW_H_

#ifdef LIBBUILD
#define LIBAPI __attribute__((__visibility__("default")))
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TFC-Debug"
#else
#define LIBAPI
#endif

// Forward declaration of TFC Core Language Features
namespace TFC {

/**
 * EventClass is an attribute class marking that the class inheriting this class may have
 * a member function as an event handler. Member function with correct signature may be
 * registered as event handler using event handler registration syntax as follow:
 */
class EventClass;

/**
 * EventClass is an attribute class marking that the class inheriting this class may have
 * polymorphic behavior which can be downcasted during the runtime. This class has virtual
 * destructor which enables the polymorphic and RTTI feature on this class and subclass.
 */
class ObjectClass;

/**
 * PropertyClass is an attribute class marking that the class can implement Property object
 * which is an accessor and mutator that can behaves like a normal field.
 */
template<typename TClass>
class PropertyClass;

/**
 * EventEmitter is an attribute class marking that the class inheriting this class may has
 * Event object declaration with this object as the source.
 */
template<typename TClass>
class EventEmitterClass;

namespace Core {

/**
 * EventObject class is the core infrastructure implementing event handling mechanism within
 * the Tizen Fundamental Classes library. It implements delegation internally which can direct
 * event handling call to the registered event handler.
 *
 * It implements += operator to register handler to the event, and -= operator to unregister.
 */
template<typename = void*, typename = void*>
class EventObject;

/**
 * SharedEventObject class is safe wrapper for EventObject class which lives outside of the
 * instance of the object declares it. It uses std::shared_ptr internally to manages the
 * instance by reference counting, thus making this EventObject is safe to be transferred
 * across stack-frame and thread boundary.
 *
 * SharedEventObject is mostly used to implement asynchronous mechanism safely which the thread
 * might not be finished when the initiator is destroyed. By using basic EventObject, it will
 * create a possible fault because EventObject lifetime is tied to the object declares it.
 */
template<typename = void*, typename = void*>
class SharedEventObject;

class PropertyObjectBase;

template<typename TDefining, typename TValue>
struct PropertyObject;

template<typename TDefining, typename TValue>
struct PropertyGetterFunction;

template<typename TDefining, typename TValue>
struct PropertySetterFunction;

}
}

#include "TFC/Core/Event.inc.h"
#include "TFC/Core/Property.inc.h"

class LIBAPI TFC::ObjectClass : public TFC::EventClass
{
public:
	virtual ~ObjectClass();
};



#endif /* CORE_NEW_H_ */
