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
public:
	char const* functionName;

	virtual bool Equals(FunctionInfo const& other) = 0;

protected:
	FunctionInfo(char const* name) : functionName(name)
	{

	}
private:

};

template<typename T>
class FunctionInfoTemplate : public FunctionInfo
{
private:
	T ptr;
public:
	FunctionInfoTemplate(char const* name, T ptr) :
		FunctionInfo(name),
		ptr(ptr)
	{

	}

	bool Equals(FunctionInfo const& other) override
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
};

class EventInfo : public ObjectClass
{

};

class PropertyInfo : public ObjectClass
{

};



class TypeInfo
{
public:
	class TypeInfoBuilder
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
		TypeInfoBuilder(T member, char const* name)
		{
			InitializerFunctor<T>::Initialize(member, name, kind, infoObject);
		}

	private:
		friend class TypeInfo;

		TypeMemberKind kind;
		ObjectClass* infoObject;
	};

	TypeInfo(std::string className, std::initializer_list<TypeInfoBuilder> init)
	{
		for(auto& i : init)
		{
			switch(i.kind)
			{
			case TypeInfoBuilder::TypeMemberKind::Function:
				auto funcInfo = dynamic_cast<FunctionInfo*>(i.infoObject);
				functionMap.emplace(std::make_pair(funcInfo->functionName, funcInfo));
				break;
			}
		}
	}



	std::map<std::string, FunctionInfo*> functionMap;

	template<typename TMemPtr>
	char const* GetFunctionNameByPointer(TMemPtr ptr)
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
private:

};

template<typename T>
struct TypeMetadata
{

};



class ServiceEndpoint
{

};

}}

template<typename T>
struct TFC::ServiceModel::TypeInfo::TypeInfoBuilder::InitializerFunctor<T, true>
{
	static void Initialize(
			T member,
			char const* name,
			TFC::ServiceModel::TypeInfo::TypeInfoBuilder::TypeMemberKind& kind,
			TFC::ObjectClass*& ptr)
	{
		ptr = new FunctionInfoTemplate<T>(name, member);
		kind = TypeMemberKind::Function;
	}
};
#endif /* TFC_SERVICEMODEL_REFLECTION_H_ */
