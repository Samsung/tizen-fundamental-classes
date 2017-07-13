/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Core/Reflection.h
 *
 * Experimental header which provides runtime reflection information by
 * declaring additional Embedded Domain-Specific Language (EDSL) which
 * defines the type information richer than C++ RTTI.
 *
 * Created on: 	Jan 23, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef TFC_SERVICEMODEL_REFLECTION_H_
#define TFC_SERVICEMODEL_REFLECTION_H_

#include "TFC/Core.h"
#include "TFC/Core/Introspect.h"
#include "TFC/Core/Invocation.h"

#include <tuple>
#include <map>
#include <string>
#include <memory>
#include <algorithm>

#include <dlog.h>

namespace TFC {
namespace Core {

TFC_ExceptionDeclare	(ReflectionException, RuntimeException);
TFC_ExceptionDeclare	(TypeNotFoundException, ReflectionException);
TFC_ExceptionDeclare	(FunctionNotFoundException, ReflectionException);

class FunctionSignatureBase
{
public:
	virtual bool Match(FunctionSignatureBase const& other) const = 0;
	virtual ~FunctionSignatureBase()
	{

	}

private:
	int nothing;
};



template<typename... Args>
class FunctionSignature : public FunctionSignatureBase
{
public:
	virtual bool Match(FunctionSignatureBase const& other) const override
	{
		try
		{
			auto res = dynamic_cast<FunctionSignature<Args...> const&>(other);
			return true;
		}
		catch(std::bad_cast const&)
		{
			return false;
		}
	}
private:
	int nothing;
};

template<typename T>
struct FunctionSignatureTupleHelper;

template<typename... TArgs>
struct FunctionSignatureTupleHelper<std::tuple<TArgs...>>
{
	typedef FunctionSignature<TArgs...> Type;
};

template<typename T>
using FunctionSignatureFromTuple = typename FunctionSignatureTupleHelper<T>::Type;

class FunctionInfo : public ObjectClass
{
private:
	size_t hash;

public:
	char const* functionName;
	virtual bool Equals(FunctionInfo const& other) const = 0;
	virtual void* Invoke(void* object, void* param) const = 0;
	size_t GetHash() { return this->hash; }

protected:
	FunctionInfo(char const* name, size_t hash) :  hash(hash), functionName(name) {	}
};

class EventInfo : public ObjectClass
{
private:
	size_t hash;

public:
	char const* eventName;
	virtual bool Equals(EventInfo const& other) const = 0;
	virtual void Raise(void* object, void* param) const = 0;
	size_t GetHash() { return this->hash; }

protected:
	EventInfo(char const* name, size_t hash) : hash(hash), eventName(name) { }
};

class ConstructorInfo : public ObjectClass
{
private:
	std::unique_ptr<FunctionSignatureBase> signature;
public:
	char const* constructorName;

	virtual bool Equals(ConstructorInfo const& other) const = 0;

	template<typename... TArgs>
	void* Construct(TArgs... param) const
	{
		auto t = std::make_tuple(param...);
		return ConstructInternal(&t);
	}

	template<typename... TArgs>
	void Throw(TArgs... param) const
	{
		auto t = std::make_tuple(param...);
		ThrowInternal(&t);
	}

	bool SignatureMatch(FunctionSignatureBase const& sig) const
	{
		return signature->Match(sig);
	}

protected:
	virtual void* ConstructInternal(void* param) const = 0;
	virtual void ThrowInternal(void* param) const = 0;
	ConstructorInfo(char const* name, FunctionSignatureBase* signaturePtr) :  signature(signaturePtr), constructorName(name)
	{

	}
};

template<typename T>
struct InvocationResult
{
	T result;
	InvocationResult(T&& var) : result(std::move(var)) { }
	virtual ~InvocationResult() { }
};

template<>
struct InvocationResult<void>
{
	virtual ~InvocationResult() { }
};

template<typename T, typename TIntrospect = Core::Introspect::MemberFunction<T>, typename TRet = typename TIntrospect::ReturnType>
struct InvocationResultTemplate : InvocationResult<TRet>
{
	typedef typename TIntrospect::DeclaringType DeclaringType;
	typedef typename TIntrospect::ArgsTuple ArgsTuple;

	InvocationResultTemplate(DeclaringType* obj, T ptr, ArgsTuple* args) :
		InvocationResult<TRet>(DelayedInvoker<T>::Invoke(obj, ptr, *args))
	{

	}
};

template<typename T, typename TIntrospect>
struct InvocationResultTemplate<T, TIntrospect, void> : InvocationResult<void>
{
	typedef typename TIntrospect::DeclaringType DeclaringType;
	typedef typename TIntrospect::ArgsTuple ArgsTuple;

	InvocationResultTemplate(DeclaringType* obj, T ptr, ArgsTuple* args)
	{
		DelayedInvoker<T>::Invoke(obj, ptr, *args);
	}
};

template<typename T>
class FunctionInfoTemplate : public FunctionInfo
{
private:
	T ptr;

	static size_t Hash(T ptr)
	{
		auto val = Core::Introspect::PointerToMemberFunction::Get(ptr);
		return Core::Introspect::PointerToMemberFunctionHash()(val);
	}
public:
	typedef typename Core::Introspect::MemberFunction<T>::ReturnType ReturnType;
	typedef typename Core::Introspect::MemberFunction<T>::DeclaringType DeclaringType;
	typedef typename Core::Introspect::MemberFunction<T>::ArgsTuple ArgsTuple;

	FunctionInfoTemplate(char const* name, T ptr) :
		FunctionInfo(name, Hash(ptr)),
		ptr(ptr)
	{

	}

	bool Equals(FunctionInfo const& other) const override
	{
		try
		{
			//dlog_print(DLOG_DEBUG, "RPC-Test", "Try cast");

			auto casted = dynamic_cast<FunctionInfoTemplate<T> const&>(other);
			if(casted.ptr == this->ptr)
				return true;
			else
				return false;
		}
		catch(std::bad_cast& t)
		{
			//dlog_print(DLOG_DEBUG, "RPC-Test", "Bad typeid");
			return false;
		}
	}

	virtual void* Invoke(void* object, void* args) const override
	{
		return new InvocationResultTemplate<T>(reinterpret_cast<DeclaringType*>(object), ptr, reinterpret_cast<ArgsTuple*>(args));
	}
};

template<typename T>
class EventInfoTemplate : public EventInfo
{
private:
	T ptr;

	static size_t Hash(T ptr)
	{
		auto val = Core::Introspect::PointerToMember::Get(ptr);
		return Core::Introspect::PointerToMemberHash()(val);
	}

public:
	typedef typename Core::Introspect::MemberEvent<T>::EventArgType EventArgType;
	typedef typename Core::Introspect::MemberEvent<T>::DeclaringType DeclaringType;

	EventInfoTemplate(char const* name, T ptr) :
		EventInfo(name, Hash(ptr)),
		ptr(ptr)
	{

	}

	bool Equals(EventInfo const& other) const override
	{
		try
		{
			//dlog_print(DLOG_DEBUG, "RPC-Test", "Try cast");

			auto casted = dynamic_cast<EventInfoTemplate<T> const&>(other);
			if(casted.ptr == this->ptr)
				return true;
			else
				return false;
		}
		catch(std::bad_cast& t)
		{
			//dlog_print(DLOG_DEBUG, "RPC-Test", "Bad typeid");
			return false;
		}
	}

	void Raise(void* object, void* param) const override
	{
		auto obj = reinterpret_cast<DeclaringType*>(object);
		auto arg = reinterpret_cast<typename std::decay<EventArgType>::type*>(param);

		(obj->*ptr)(obj, *arg);
	}
};

template<typename TInfo>
class ConstructorInfoTemplate : public ConstructorInfo
{
private:
	typedef typename TInfo::Type Type;
	typedef typename TInfo::ArgsTuple ArgsTuple;

public:
	void* ConstructInternal(void* params) const override
	{
		return DelayedConstructor<Type, ArgsTuple>::Invoke(*reinterpret_cast<ArgsTuple*>(params));
	}

	void ThrowInternal(void* params) const override
	{
		DelayedConstructor<Type, ArgsTuple>::Throw(*reinterpret_cast<ArgsTuple*>(params));
	}

	virtual bool Equals(ConstructorInfo const& other) const override
	{
		try
		{
			//dlog_print(DLOG_DEBUG, "RPC-Test", "Try cast");
			(void)(dynamic_cast<ConstructorInfoTemplate<TInfo> const&>(other));
			return true;
		}
		catch(std::bad_cast& t)
		{
			//dlog_print(DLOG_DEBUG, "RPC-Test", "Bad typeid");
			return false;
		}
	}

	ConstructorInfoTemplate(char const* name) : ConstructorInfo(name, new FunctionSignatureFromTuple<ArgsTuple>())
	{

	}
};

class PropertyInfo : public ObjectClass
{

};

enum class TypeMemberKind
{
	Function,
	Event,
	Property,
	Constructor,
	Destructor
};

template<typename T, typename = void>
struct TypeMemberKindSelector;

template<typename TClass, typename... TArgs>
struct Constructor
{
	typedef TClass Type;
	typedef std::tuple<TArgs...> ArgsTuple;
};

template<typename TClass>
struct Destructor
{
	typedef TClass Type;
};

class DestructorInfo : public ObjectClass
{
public:
	virtual void Delete(void* ptr) const = 0;
	virtual void Destruct(void* ptr) const = 0;
};

template<typename TClass>
class DestructorInfoTemplate : public DestructorInfo
{
public:

	void Delete(void* ptr) const override
	{
		delete reinterpret_cast<TClass*>(ptr);
	}

	void Destruct(void* ptr) const override
	{
		reinterpret_cast<TClass*>(ptr)->~TClass();
	}
};


class TypeDescription
{
public:
	class TypeDescriptionBuilder
	{
	public:
		template<typename T, TypeMemberKind = TypeMemberKindSelector<T>::value>
		struct InitializerFunctor;

		template<typename T>
		TypeDescriptionBuilder(T member, char const* name = "")
		{
			InitializerFunctor<T>::Initialize(member, name, kind, infoObject);
		}

		template<typename T>
		struct InitializerFunctor<T, TypeMemberKind::Function>
		{
			static void Initialize(
					T member,
					char const* name,
					TypeMemberKind& kind,
					ObjectClass*& ptr)
			{
				ptr = new FunctionInfoTemplate<T>(name, member);
				kind = TypeMemberKind::Function;
			}
		};

		template<typename T>
		struct InitializerFunctor<T, TypeMemberKind::Constructor>
		{
			static void Initialize(
					T member,
					char const* name,
					TypeMemberKind& kind,
					ObjectClass*& ptr)
			{
				ptr = new ConstructorInfoTemplate<T>(name);
				kind = TypeMemberKind::Constructor;
			}
		};

		template<typename T>
		struct InitializerFunctor<T, TypeMemberKind::Destructor>
		{
			static void Initialize(
					T member,
					char const* name,
					TypeMemberKind& kind,
					ObjectClass*& ptr)
			{
				ptr = new DestructorInfoTemplate<typename T::Type>();
				kind = TypeMemberKind::Destructor;
			}
		};

		template<typename T>
		struct InitializerFunctor<T, TypeMemberKind::Event>
		{
			static void Initialize(
					T member,
					char const* name,
					TypeMemberKind& kind,
					ObjectClass*& ptr)
			{
				//ptr = new DestructorInfoTemplate<typename T::Type>();
				//dlog_print(DLOG_DEBUG, "RPC-Test", "Initialize event: %s", name);
				ptr = new EventInfoTemplate<T>(name, member);
				kind = TypeMemberKind::Event;
			}
		};


	private:
		friend class TypeDescription;

		TypeMemberKind kind;
		ObjectClass* infoObject;
	};

	TypeDescription(std::initializer_list<TypeDescriptionBuilder>& init, std::type_info const& info);

	std::map<std::string, FunctionInfo*> functionMap;
	std::map<std::string, ConstructorInfo*> constructorMap;
	std::map<std::string, FunctionInfo*> const& GetFunctionMap() const { return functionMap; }
	DestructorInfo* destructor;
	std::map<std::string, EventInfo*> eventMap;

	template<typename TMemPtr>
	char const* GetFunctionNameByPointer(TMemPtr ptr) const
	{
		FunctionInfoTemplate<TMemPtr> prototype(nullptr, ptr);

		//dlog_print(DLOG_DEBUG, "RPC-Test", "GetFunctionNameByPointer");

		auto res = std::find_if(functionMap.begin(), functionMap.end(), [&] (typename decltype(functionMap)::value_type const& w)
		{
			if(w.second->Equals(prototype))
				return true;
			else
				return false;
		});

		if(res == functionMap.end())
			throw FunctionNotFoundException("Function specified is not found");

		return res->second->functionName;
	}

	template<typename TEvPtr>
	char const* GetEventNameByPointer(TEvPtr ptr) const
	{
		EventInfoTemplate<TEvPtr> prototype(nullptr, ptr);

		dlog_print(DLOG_DEBUG, "RPC-Test", "GetEventNameByPointer");

		auto res = std::find_if(eventMap.begin(), eventMap.end(), [&] (typename decltype(eventMap)::value_type const& w)
		{
			if(w.second->Equals(prototype))
				return true;
			else
				return false;
		});

		if(res == eventMap.end())
			throw FunctionNotFoundException("Event specified is not found");

		return res->second->eventName;
	}

	FunctionInfo const& GetFunctionByName(std::string const& name) const
	{
		auto res = functionMap.find(name);

		if(res == functionMap.end())
					throw FunctionNotFoundException("Function specified is not found");

		return *res->second;
	}

	template<typename... TArgs>
	ConstructorInfo const& GetConstructor() const
	{
		FunctionSignature<TArgs...> sigs;

		for(auto& constructor : constructorMap)
		{
			if(constructor.second->SignatureMatch(sigs))
				return *constructor.second;
		}

		throw FunctionNotFoundException("Cannot find specified constructor.");
	}

	template<typename... TArgs>
	void* Construct(TArgs... args) const
	{
		auto& consInfo = GetConstructor<TArgs...>();
		return consInfo.Construct(args...);
	}

	template<typename... TArgs>
	void Throw(TArgs... args) const
	{
		auto& consInfo = GetConstructor<TArgs...>();
		consInfo.Throw(args...);
	}

	void Delete(void* param) const
	{
		if(destructor == nullptr)
			throw FunctionNotFoundException("No destructor is available");

		this->destructor->Delete(param);
	}
private:

};

template<typename T>
class TypeDescriptionTemplate : public TypeDescription
{
public:
	TypeDescriptionTemplate(std::initializer_list<TypeDescriptionBuilder> init) :
		TypeDescription(init, typeid(T))
	{

	}
};

template<typename T>
struct TypeInfo
{
	static TypeDescriptionTemplate<T> typeDescription;
};

TypeDescription const& FindTypeByName(std::string const& name);

}}

template<typename T>
struct TFC::Core::TypeMemberKindSelector<T, typename std::enable_if<TFC::Core::Introspect::IsMemberFunction<T>::Value>::type>
{
	static constexpr TypeMemberKind value = TypeMemberKind::Function;
};

template<typename... TArgs>
struct TFC::Core::TypeMemberKindSelector<TFC::Core::Constructor<TArgs...>, void>
{
	static constexpr TypeMemberKind value = TypeMemberKind::Constructor;
	typedef std::tuple<TArgs...> TupleArgs;
};

template<typename T>
struct TFC::Core::TypeMemberKindSelector<TFC::Core::Destructor<T>, void>
{
	static constexpr TypeMemberKind value = TypeMemberKind::Destructor;
};

template<typename TDeclaring, typename TArg>
struct TFC::Core::TypeMemberKindSelector<TFC::Core::EventObject<TDeclaring*, TArg> TDeclaring::*, void>
{
	static constexpr TypeMemberKind value = TypeMemberKind::Event;
};


#define TFC_DefineTypeInfo(TYPENAME) \
	template<> __attribute__((visibility("default")))\
	TFC::Core::TypeDescriptionTemplate < TYPENAME > TFC::Core::TypeInfo< TYPENAME >::typeDescription

#define TFC_DefineExceptionInfo(EXCEPTIONNAME) \
	TFC_DefineTypeInfo(EXCEPTIONNAME) { \
		{ TFC::Core::Constructor< EXCEPTIONNAME >(), "Default" }, \
		{ TFC::Core::Constructor< EXCEPTIONNAME , char const*>(), "CStr" }, \
		{ TFC::Core::Constructor< EXCEPTIONNAME , std::string&&>(), "StringRvalRef" }, \
		{ TFC::Core::Constructor< EXCEPTIONNAME , std::string const&>(), "StringConstRef" } \
	}

#endif /* TFC_SERVICEMODEL_REFLECTION_H_ */
