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

#include <exception>
#include <string>

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
 * which is an accessor and mutator that can behaves like a normal field. PropertyClass should be
 * inherited as private inheritance because it acts as mixin which introduces Property name inside
 * the class scope.
 * ```
 *    class ClassA : TFC::PropertyClass<ClassA>
 *    {
 *    public:
 *        int valA;
 *
 *        int GetA() const
 *        {
 *            return valA;
 *        }
 *
 *        void SetA(int const& val)
 *        {
 *            this->valA = val;
 *        }
 *
 *        Property<int>::GetSet<&ClassA::GetA, &ClassA::SetA> A;
 *     };
 * ```
 * When using PropertyClass attribute alongside, with a base class that already declare
 * PropertyClass attribute, Property name must be reintroduced in its subclass scope by calling
 * `using TFC::PropertyClass<CLASS_NAME>::Property`.
 *
 * ```
 *     class ClassB : public ClassA, TFC::PropertyClass<ClassB>
 *     {
 *         using TFC::PropertyClass<ClassB>::Property;
 *     public:
 *         int valB;
 *         int GetB() const
 *         {
 *             return valB;
 *         }
 *
 *         void SetB(int const& val)
 *         {
 *             this->valB = val;
 *         }
 *
 *         Property<int>::GetSet<&ClassB::GetB, &ClassB::SetB> B;
 *     };
 * ```
 *
 * @tparam TClass The class itself
 */
template<typename TClass>
class PropertyClass;

/**
 * EventEmitter is an attribute class marking that the class inheriting this class may has
 * Event object declaration with this object as the source.
 */
template<typename TClass>
class EventEmitterClass;

class LIBAPI TFCException : public std::exception
{
public:
	explicit TFCException(char const* message);
	explicit TFCException(std::string&& message);
	explicit TFCException(std::string const& message);
	virtual char const* what() const throw () final;

private:
	std::string msg;
};

struct Color
{
	uint8_t r, g, b, a;
	static inline Color FromRGBA(uint32_t val) { return *(reinterpret_cast<Color*>(&val)); }
};

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

#define EXCEPTION_DECLARE(NAME, PARENT)\
class NAME : public PARENT\
{\
public:\
	explicit inline NAME(char const* message) : PARENT(message) { }\
	explicit inline NAME(std::string&& message) : PARENT(message) { }\
	explicit inline NAME(std::string const& message) : PARENT(message) { }\
}

#define EXCEPTION_DECLARE_MSG(NAME, PARENT, DEFAULT_MESSAGE)\
class NAME : public PARENT\
{\
public:\
	explicit inline NAME() : PARENT(DEFAULT_MESSAGE) { }\
	explicit inline NAME(char const* message) : PARENT(message) { }\
	explicit inline NAME(std::string&& message) : PARENT(message) { }\
	explicit inline NAME(std::string const& message) : PARENT(message) { }\
}

#endif /* CORE_NEW_H_ */
