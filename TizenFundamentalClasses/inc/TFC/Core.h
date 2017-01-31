/*
 * Core.h
 *
 *  Created on: Oct 25, 2016
 *      Author: Gilang M. Hamidy
 */

#pragma once

#ifndef TFC_CORE_H_
#define TFC_CORE_H_

#ifdef LIBBUILD
#define LIBAPI __attribute__((__visibility__("default")))
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TFC-Debug"
#else
#define LIBAPI
#endif

#include <memory>
#include <exception>
#include <string>
#include <type_traits>


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
class LIBAPI ObjectClass
{
public:
	virtual ~ObjectClass();
};

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
 *        Property<int>::Get<&ClassA::GetA>::Set<&ClassA::SetA> A;
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
 *         Property<int>::Get<&ClassB::GetB>::Set<&ClassB::SetB> B;
 *     };
 * ```
 *
 * @tparam TClass The class itself
 */
template<typename TClass>
class PropertyClass;

class ManagedClass
{
public:
	class SharedHandle;

	class SafePointer
	{
	public:
		bool TryAccess() const;
		SafePointer();
		~SafePointer();
		SafePointer(SafePointer&& that);
		SafePointer(SafePointer const& that);
		operator bool() const { return TryAccess(); }
	private:
		friend class ManagedClass;
		SafePointer(SharedHandle* handle);


		SharedHandle* handle;
	};

	template<typename T>
	static SafePointer GetSafePointerFrom(T* what);

	template<typename T, bool = std::is_base_of<ManagedClass, T>::value>
	struct SafePointerGetter;

	ManagedClass();
	~ManagedClass();
	SafePointer GetSafePointer();

private:
	SharedHandle* handle;

};

template<typename T, bool>
struct ManagedClass::SafePointerGetter
{
	static SafePointer GetSafePointer(T* what)
	{
		return what->GetSafePointer();
	}
};

template<typename T>
struct ManagedClass::SafePointerGetter<T, false>
{
	static SafePointer GetSafePointer(T* what)
	{
		return {};
	}
};

/**
 * EventEmitter is an attribute class marking that the class inheriting this class may has
 * Event object declaration with this object as the source.
 */
template<typename TClass>
class EventEmitterClass;

class LIBAPI TFCException : public std::exception
{
public:
	explicit TFCException() { }
	explicit TFCException(char const* message);
	explicit TFCException(std::string&& message);
	explicit TFCException(std::string const& message);
	virtual char const* what() const throw () final;

private:
	std::string msg;
	std::string stackTrace;

	void BuildStackTrace();

public:
	std::string& GetStackTrace() { return this->stackTrace; }
};

struct Color
{
	uint8_t r, g, b, a;
	static inline Color FromRGBA(uint32_t val) { return *(reinterpret_cast<Color*>(&val)); }
};

template<typename T>
inline bool IsNull(T* ptr)
{
	return ptr == nullptr ? true : false;
}

template<typename T>
inline T* Coalesce(T* ptr, T* valueIfNull)
{
	return IsNull(ptr) ? ptr : valueIfNull;
}

template<typename TExcept, typename T>
inline T* ThrowIfNull(T* ptr)
{
	if(ptr == nullptr)
		throw TExcept{};

	return ptr;
}

template<typename TExcept = TFC::TFCException>
inline void TFCAssert(bool value, char const* message = "")
{
	if(!value)
		throw TExcept(message);
}

template<typename TExcept = TFC::TFCException>
inline void TFCAssertZero(int value, char const* message = "")
{
	if(value != 0)
	{
		std::string msg(message);
		msg += " (Assertion failed: ";
		msg += std::to_string(value);
		msg += ")";
		throw TExcept(std::move(msg));
	}
}

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

template<typename T>
struct ObjectClassPackage : public ObjectClass
{
	T value;

	template<typename TArgs>
	ObjectClassPackage(TArgs param) : value(std::forward<TArgs>(param))
	{

	}
};

template<typename T>
ObjectClassPackage<T>* PackInObjectClass(T value)
{
	return new ObjectClassPackage<typename std::remove_reference<T>::type> { std::forward<T>(value) };
}

template<typename T>
T UnpackFromObjectClass(ObjectClass* obj)
{
	auto ptrRaw = dynamic_cast<ObjectClassPackage<T>*>(obj);
	if(ptrRaw == nullptr)
		throw TFCException("Invalid package retrieved in UnpackFromObjectClass");
	std::unique_ptr<ObjectClassPackage<T>> ptr(ptrRaw);

	return ptrRaw->value;
}



}
}

#define TFC_ExceptionDeclare(NAME, PARENT)\
class NAME : public PARENT\
{\
public:\
	explicit inline NAME() : PARENT() { }\
	explicit inline NAME(char const* message) : PARENT(message) { }\
	explicit inline NAME(std::string&& message) : PARENT(message) { }\
	explicit inline NAME(std::string const& message) : PARENT(message) { }\
}

#define TFC_ExceptionDeclareWithMessage(NAME, PARENT, DEFAULT_MESSAGE)\
class NAME : public PARENT\
{\
public:\
	explicit inline NAME() : PARENT(DEFAULT_MESSAGE) { }\
	explicit inline NAME(char const* message) : PARENT(message) { }\
	explicit inline NAME(std::string&& message) : PARENT(message) { }\
	explicit inline NAME(std::string const& message) : PARENT(message) { }\
}


namespace TFC {

TFC_ExceptionDeclare	(RuntimeException, TFCException);
TFC_ExceptionDeclare	(ArgumentException, TFCException);

namespace Core {

TFC_ExceptionDeclare	(InvocationException, RuntimeException);

}
}

#include "TFC/Core/Event.inc.h"
#include "TFC/Core/Property.inc.h"

template<typename T>
TFC::ManagedClass::SafePointer TFC::ManagedClass::GetSafePointerFrom(T* what)
{
	return TFC::ManagedClass::SafePointerGetter<T>::GetSafePointer(what);
}


#endif /* CORE_NEW_H_ */
