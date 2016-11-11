/*
 * Introspect.h
 *
 *  Created on: Oct 31, 2016
 *      Author: Gilang M. Hamidy
 */
#pragma once

#ifndef TFC_CORE_INTROSPECT_H_
#define TFC_CORE_INTROSPECT_H_

#include <type_traits>
#include "TFC/Core.h"

namespace TFC {
namespace Core {
namespace Introspect {

template<typename TClass, typename TEventInfo>
EventObject<TClass*, TEventInfo> EventOfFunc(EventObject<TClass*, TEventInfo> TClass::* param);

template<typename T>
class HasMemberAccessOperator
{
	typedef char Correct;
	typedef long Incorrect;
	template<typename TTest> static Correct Test(decltype(&TTest::operator->));
	template<typename TTest> static Incorrect Test(...);

public:
	static constexpr bool Value = sizeof(Test<T>(0)) == sizeof(Correct);
};

template<typename T>
class HasCallOperator
{
	typedef char Correct;
	typedef long Incorrect;
	template<typename TTest> static Correct Test(decltype(&TTest::operator()));
	template<typename TTest> static Incorrect Test(...);

public:
	static constexpr bool Value = sizeof(Test<T>(0)) == sizeof(Correct);
};



template<typename TFuncType>
struct MemberFunction
{

};

template<typename TClass, typename TReturn, typename... TArgs>
struct MemberFunction<TReturn (TClass::*)(TArgs...)>
{
	static constexpr auto Arity = sizeof...(TArgs);
	typedef TReturn ReturnType;
	typedef TClass	DeclaringType;

	template<size_t idx>
	using Args = typename std::tuple_element<idx, std::tuple<TArgs...>>::type;
};

template<typename TClass, typename TReturn, typename... TArgs>
struct MemberFunction<TReturn (TClass::*)(TArgs...) const> : MemberFunction<TReturn (TClass::*)(TArgs...)>
{
	static constexpr auto Arity = sizeof...(TArgs);
	typedef TReturn ReturnType;
	typedef TClass	DeclaringType;

	template<size_t idx>
	using Args = typename std::tuple_element<idx, std::tuple<TArgs...>>::type;
};

template<typename T>
struct StaticFunction
{

};

template<typename TRet, typename... TArgs>
struct StaticFunction<TRet(TArgs...)>
{
	static constexpr auto Arity = sizeof...(TArgs);
	typedef TRet ReturnType;

	template<size_t idx>
	using Args = typename std::tuple_element<idx, std::tuple<TArgs...>>::type;
};

template<typename T>
struct IsFunctionPointer
{
	static constexpr bool Value = false;
};

template<typename TRet, typename... TArgs>
struct IsFunctionPointer<TRet(*)(TArgs...)>
{
	static constexpr bool Value = true;
};

template<typename T, bool = HasCallOperator<T>::Value || std::is_function<typename std::remove_pointer<T>::type>::value>
struct CallableObject;


template<typename T>
struct CallableObject<T, false>
{
	static constexpr bool Callable = false; // Fallback
};

template<typename T>
struct CallableObject<T, true> : MemberFunction<decltype(&T::operator())>
{
	static constexpr bool Callable = true;
	typedef typename MemberFunction<decltype(&T::operator())>::ReturnType ReturnType;
};

template<typename TRet, typename... TArgs>
struct CallableObject<TRet(*)(TArgs...), true> : StaticFunction<TRet(TArgs...)>
{
	static constexpr bool Callable = true;
	typedef typename StaticFunction<TRet(TArgs...)>::ReturnType ReturnType;
};



}}}

#define EventOf(EVENT_VAR) decltype(TFC::Core::Introspect::EventOfFunc(& EVENT_VAR))


#endif /* INTROSPECT_H_ */
