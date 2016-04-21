/*
 * Core.h
 *
 *  Created on: Feb 12, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef CORE_H_
#define CORE_H_

#include <dlog.h>
#include <functional>
#include <memory>

#ifdef LIBBUILD
#define LIBAPI __attribute__((__visibility__("default")))
#define LOG_TAG "SRINFW"
#else
#define LIBAPI
#endif

typedef const char* CString;

//extern thread_local void* currentConstruct;

/*
 * http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda
 */
template<typename T>
struct function_traits: public function_traits<decltype(&T::operator())>
{
};

template<typename ClassType, typename ReturnType, typename ... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
{
	enum
	{
		arity = sizeof...(Args)
	};

	typedef ReturnType result_type;

	template <size_t i>
	struct arg
	{
		typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
	};
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


class PropertyClass
{
private:

	class PropertyBase
	{
	protected:
		void* instance;
		PropertyBase(void* instance);
		PropertyBase(const PropertyBase&) = delete;
	};

public:
	template<typename DefiningClass, typename ValueType>
	struct Property
	{
		typedef ValueType (DefiningClass::*GetFunc)();
		typedef void (DefiningClass::*SetFunc)(const ValueType&);

		template<GetFunc func>
		class Get : protected PropertyBase
		{
		public:
			typedef std::false_type Mutable;
			typedef ValueType Type;
			Get(DefiningClass* instance) : PropertyBase(instance) { }
			operator ValueType() const
			{
				return (reinterpret_cast<DefiningClass*>(instance)->*func)();
			}
		};

		template<GetFunc getFunc, SetFunc func>
		class GetSet : public Get<getFunc>
		{
		public:
			typedef std::true_type Mutable;
			typedef ValueType Type;
			GetSet(DefiningClass* instance) : Get<getFunc>(instance) { }
			void operator=(const ValueType val)
			{
				(reinterpret_cast<DefiningClass*>(this->instance)->*func)(val);
			}
		};

		class Auto
		{
		private:
			ValueType data;
		public:
			class ReadOnly;
			class ReadWrite;
		};
	};

	template<typename DefiningClass, typename ValueType>
	struct Property<DefiningClass, ValueType&>
	{
		typedef ValueType& (DefiningClass::*GetFunc)();
		typedef void (DefiningClass::*SetFunc)(const ValueType&);

		template<GetFunc func>
		class Get : protected PropertyBase
		{
		public:
			typedef std::false_type Mutable;
			typedef ValueType Type;
			Get(DefiningClass* instance) : PropertyBase(instance) { }
			operator ValueType&() const
			{
				return (reinterpret_cast<DefiningClass*>(this->instance)->*func)();
			}
			ValueType* operator->()
			{
				return &((reinterpret_cast<DefiningClass*>(this->instance)->*func)());
			}
		};

		template<GetFunc getFunc, SetFunc func>
		class GetSet : public Get<getFunc>
		{
		public:
			typedef std::true_type Mutable;
			typedef ValueType Type;
			GetSet(DefiningClass* instance) : Get<getFunc>(instance) { }
			void operator=(const ValueType& val)
			{
				(reinterpret_cast<DefiningClass*>(this->instance)->*func)(val);
			}
		};
	};

	template<typename DefiningClass, typename ValueType>
	struct Property<DefiningClass, ValueType*>
	{
		typedef ValueType* (DefiningClass::*GetFunc)();
		typedef void (DefiningClass::*SetFunc)(ValueType*);

		template<GetFunc func>
		class Get : protected PropertyBase
		{
		public:
			typedef std::false_type Mutable;
			typedef ValueType Type;
			Get(DefiningClass* instance) : PropertyBase(instance) { }
			operator ValueType*()
			{
				return (reinterpret_cast<DefiningClass*>(this->instance)->*func)();
			}
			ValueType* operator->()
			{
				return (reinterpret_cast<DefiningClass*>(this->instance)->*func)();
			}
		};

		template<GetFunc getFunc, SetFunc func>
		class GetSet : public Get<getFunc>
		{
		public:
			typedef std::true_type Mutable;
			typedef ValueType Type;
			GetSet(DefiningClass* instance) : Get<getFunc>(instance) { }
			void operator=(ValueType* val)
			{
				(reinterpret_cast<DefiningClass*>(this->instance)->*func)(val);
			}
		};

		class Auto
		{
		private:
			ValueType* data;
		public:
			class ReadOnly;
			class ReadWrite;
		};
	};
};

template<typename DefiningClass, typename ValueType>
class PropertyClass::Property<DefiningClass, ValueType>::Auto::ReadOnly : Auto
{
protected:
	void operator=(const ValueType& val)
	{
		this->data = val;
	}

	friend DefiningClass;
public:
	typedef std::false_type Mutable;
	typedef ValueType Type;
	operator ValueType()
	{
		return this->data;
	}
	ValueType* operator->()
	{
		return &this->data;
	}
};

template<typename DefiningClass, typename ValueType>
class PropertyClass::Property<DefiningClass, ValueType>::Auto::ReadWrite : public ReadOnly
{
public:
	typedef std::true_type Mutable;
	typedef ValueType Type;
	void operator=(const ValueType& val)
	{
		ReadOnly::operator =(val);
	}
};

template<typename DefiningClass, typename ValueType>
class PropertyClass::Property<DefiningClass, ValueType*>::Auto::ReadOnly : Auto
{
protected:
	void operator=(ValueType* val)
	{
		this->data = val;
	}

	friend DefiningClass;
public:
	typedef std::false_type Mutable;
	typedef ValueType Type;
	operator ValueType*()
	{
		return this->data;
	}
	ValueType* operator->()
	{
		return this->data;
	}
};

template<typename DefiningClass, typename ValueType>
class PropertyClass::Property<DefiningClass, ValueType*>::Auto::ReadWrite : public ReadOnly
{
public:
	typedef std::true_type Mutable;
	typedef ValueType Type;
	void operator=(ValueType* val)
	{
		ReadOnly::operator =(val);
	}

};

/*
template<class DefiningClass, class ValueType, ValueType& (DefiningClass::*GetFunc)(), void (DefiningClass::*SetFunc)(
	const ValueType&)>
class Property
{
private:
	DefiningClass* instance;
public:
	Property(DefiningClass* inst) :
		instance(inst)
	{
	}
	operator ValueType()
	{
		return (instance->*GetFunc)();
	}
	void operator=(const ValueType& val)
	{
		(instance->*SetFunc)(val);
	}
	ValueType* operator->() const
	{
		auto ret = (instance->*GetFunc)();
		return &ret;
	}
};

template<class DefiningClass, class ValueType, ValueType DefiningClass::* LocalVar>
class SimpleProperty
{
private:
	DefiningClass* instance;
public:
	SimpleProperty(DefiningClass* inst) :
		instance(inst)
	{
	}
	operator ValueType()
	{
		return instance->*LocalVar;
	}
	void operator=(const ValueType& val)
	{
		instance->*LocalVar = (val);
	}
};

template<class DefiningClass, class ValueType, ValueType (DefiningClass::*GetFunc)()>
class ReadOnlyProperty
{
private:
	DefiningClass* instance;
public:
	ReadOnlyProperty(DefiningClass* inst) :
		instance(inst)
	{
	}
	operator ValueType()
	{
		auto val = (instance->*GetFunc)();
		return val;
	}
};
*/
template<class DefiningClass, class ValueType>
class SimpleReadOnlyPropertyBase
{
protected:
	ValueType value;
public:
	friend DefiningClass;
	operator ValueType()
	{
		return value;
	}
	ValueType* operator->() const;
};

template<class DefiningClass, class ValueType>
class SimpleReadOnlyProperty: public SimpleReadOnlyPropertyBase<DefiningClass, ValueType>
{
private:
	void operator=(const ValueType& val)
	{
		this->value = (val);
	}
public:
	friend DefiningClass;
	const ValueType* operator->() const;
	operator ValueType()
	{
		return this->value;
	}
};

template<class DefiningClass, class ValueType>
const ValueType* SimpleReadOnlyProperty<DefiningClass, ValueType>::operator->() const
{
	return &this->value;
}

template<class DefiningClass, class ValueType>
class SimpleReadOnlyProperty<DefiningClass, ValueType*> : public SimpleReadOnlyPropertyBase<DefiningClass, ValueType*>
{
private:
	void operator=(ValueType* val)
	{
		this->value = (val);
	}
public:
	friend DefiningClass;
	ValueType* operator->() const;
};

template<class DefiningClass, class ValueType>
ValueType* SimpleReadOnlyProperty<DefiningClass, ValueType*>::operator->() const
{
	return this->value;
}

class EventClass
{
};

template<class ObjectSourceType = void*, class EventDataType = void*>
class Event
{
public:
	typedef void (EventClass::*EventHandler)(const Event<ObjectSourceType, EventDataType>* eventSource,
		ObjectSourceType objSource, EventDataType eventData);

	class EventDelegate
	{
		EventClass* instance;
		EventHandler eventHandler;
	public:
		template<class EventClassType>
		EventDelegate(EventClassType* instance,
				void (EventClassType::*eventHandler)(Event<ObjectSourceType, EventDataType>* eventSource,
					ObjectSourceType objSource, EventDataType eventData));

		friend class Event;
	};

	Event();
	Event(bool logDelete);
	~Event();
	void operator+=(const EventDelegate& other);
	void operator-=(const EventDelegate& other);
	void operator()(ObjectSourceType objSource, EventDataType eventData) const;

private:
	struct EventNode
	{
		EventClass* instance;
		EventHandler eventHandler;
		EventNode* next;
	};
	EventNode* first;
	bool logDelete;
};

template<class ObjectSourceType = void*, class EventDataType = void*>
class SharedEvent : protected std::shared_ptr<Event<ObjectSourceType, EventDataType>>
{
public:
	SharedEvent();
	void operator+=(const typename Event<ObjectSourceType, EventDataType>::EventDelegate& other);
	void operator-=(const typename Event<ObjectSourceType, EventDataType>::EventDelegate& other);
	void operator()(ObjectSourceType objSource, EventDataType eventData) const;
};

template<class ObjectSourceType, class EventDataType>
SharedEvent<ObjectSourceType, EventDataType>::SharedEvent() :
	std::shared_ptr<Event<ObjectSourceType, EventDataType>>(new Event<ObjectSourceType, EventDataType>(true))
{

}

template<class ObjectSourceType, class EventDataType>
void SharedEvent<ObjectSourceType, EventDataType>::operator+=(const typename Event<ObjectSourceType, EventDataType>::EventDelegate& other)
{
	std::shared_ptr<Event<ObjectSourceType, EventDataType>>::operator->()->operator +=(other);
}

template<class ObjectSourceType, class EventDataType>
void SharedEvent<ObjectSourceType, EventDataType>::operator-=(const typename Event<ObjectSourceType, EventDataType>::EventDelegate& other)
{
	std::shared_ptr<Event<ObjectSourceType, EventDataType>>::operator->()->operator -=(other);
}

template<class ObjectSourceType, class EventDataType>
void SharedEvent<ObjectSourceType, EventDataType>:: operator()(ObjectSourceType objSource, EventDataType eventData) const
{
	std::shared_ptr<Event<ObjectSourceType, EventDataType>>::operator->()->operator ()(objSource, eventData);
}

#include "SRIN/Core.inc"

#define AddEventHandler(EVENT_METHOD) { this, & EVENT_METHOD }

#endif /* CORE_H_ */
