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
#include "TFC/ServiceModel/InterfaceInspector.h"

#include <tuple>
#include <map>
#include <string>
#include <memory>
#include <algorithm>

#include <dlog.h>

namespace TFC {
namespace ServiceModel {

class FunctionSignatureBase
{
public:
	virtual bool Match(FunctionSignatureBase* other) = 0;
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
	virtual bool Match(FunctionSignatureBase* other) override
	{
		auto res = dynamic_cast<FunctionSignature<Args...>>(other);

		if(res == nullptr)
			return false;
		else
			return true;
	}
private:
	int nothing;
};

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
	FunctionInfo(char const* name, size_t hash) : functionName(name), hash(hash)
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

class EventInfo : public ObjectClass
{

};

class PropertyInfo : public ObjectClass
{

};



class TypeDescription
{
public:
	class TypeDescriptionBuilder
	{
	public:
		enum class TypeMemberKind
		{
			Function,
			Event,
			Property
		};

		template<typename T, bool = TFC::Core::Introspect::IsMemberFunction<T>::Value>
		struct InitializerFunctor;

		template<typename T>
		TypeDescriptionBuilder(T member, char const* name)
		{
			InitializerFunctor<T>::Initialize(member, name, kind, infoObject);
		}

	private:
		friend class TypeDescription;

		TypeMemberKind kind;
		ObjectClass* infoObject;
	};

	TypeDescription(std::initializer_list<TypeDescriptionBuilder> init)
	{
		for(auto& i : init)
		{
			switch(i.kind)
			{
			case TypeDescriptionBuilder::TypeMemberKind::Function:
				auto funcInfo = dynamic_cast<FunctionInfo*>(i.infoObject);
				functionMap.emplace(std::make_pair(funcInfo->functionName, funcInfo));
				break;
			}
		}
	}



	std::map<std::string, FunctionInfo*> functionMap;

	std::map<std::string, FunctionInfo*> const& GetFunctionMap() const { return functionMap; }

	template<typename TMemPtr>
	char const* GetFunctionNameByPointer(TMemPtr ptr) const
	{
		FunctionInfoTemplate<TMemPtr> prototype(nullptr, ptr);

		dlog_print(DLOG_DEBUG, "RPC-Test", "GetFunctionNameByPointer");

		auto res = std::find_if(functionMap.begin(), functionMap.end(), [&] (decltype(functionMap)::value_type const& w)
		{
			if(w.second->Equals(prototype))
				return true;
			else
				return false;
		});

		if(res == functionMap.end())
			dlog_print(DLOG_DEBUG, "RPC-Test", "Failed to get function");



		return res->second->functionName;
	}

	FunctionInfo const& GetFunctionByName(std::string const& name) const
	{
		auto res = functionMap.find(name);
		return *res->second;
	}
private:

};

template<typename T>
struct TypeInfo
{
	static TypeDescription typeDescription;
};

class ServiceEndpoint
{

};

}}

template<typename T>
struct TFC::ServiceModel::TypeDescription::TypeDescriptionBuilder::InitializerFunctor<T, true>
{
	static void Initialize(
			T member,
			char const* name,
			TFC::ServiceModel::TypeDescription::TypeDescriptionBuilder::TypeMemberKind& kind,
			TFC::ObjectClass*& ptr)
	{
		ptr = new FunctionInfoTemplate<T>(name, member);
		kind = TypeMemberKind::Function;
	}
};


#define TFC_DefineTypeInfo(TYPENAME) \
	template<>\
	TFC::ServiceModel::TypeDescription TFC::ServiceModel::TypeInfo< TYPENAME >::typeDescription

#endif /* TFC_SERVICEMODEL_REFLECTION_H_ */
