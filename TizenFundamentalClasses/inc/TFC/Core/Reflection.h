/*
 * Reflection.h
 *
 *  Created on: Dec 23, 2016
 *      Author: gilang
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
	FunctionInfo(char const* name, size_t hash) :  hash(hash), functionName(name)
	{

	}
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
			dlog_print(DLOG_DEBUG, "RPC-Test", "Try cast");

			auto casted = dynamic_cast<FunctionInfoTemplate<T> const&>(other);
			if(casted.ptr == this->ptr)
				return true;
			else
				return false;
		}
		catch(std::bad_cast& t)
		{
			dlog_print(DLOG_DEBUG, "RPC-Test", "Bad typeid");
			return false;
		}
	}

	virtual void* Invoke(void* object, void* args) const override
	{
		return new InvocationResultTemplate<T>(reinterpret_cast<DeclaringType*>(object), ptr, reinterpret_cast<ArgsTuple*>(args));
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
			dlog_print(DLOG_DEBUG, "RPC-Test", "Try cast");
			(void)(dynamic_cast<ConstructorInfoTemplate<TInfo> const&>(other));
			return true;
		}
		catch(std::bad_cast& t)
		{
			dlog_print(DLOG_DEBUG, "RPC-Test", "Bad typeid");
			return false;
		}
	}

	ConstructorInfoTemplate(char const* name) : ConstructorInfo(name, new FunctionSignatureFromTuple<ArgsTuple>())
	{

	}
};

class EventInfo : public ObjectClass
{

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

	template<typename TMemPtr>
	char const* GetFunctionNameByPointer(TMemPtr ptr) const
	{
		FunctionInfoTemplate<TMemPtr> prototype(nullptr, ptr);

		dlog_print(DLOG_DEBUG, "RPC-Test", "GetFunctionNameByPointer");

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
