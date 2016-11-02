/*
 * Introspect.h
 *
 *  Created on: Oct 31, 2016
 *      Author: Gilang M. Hamidy
 */

#ifndef TFC_CORE_INTROSPECT_H_
#define TFC_CORE_INTROSPECT_H_

#include <type_traits>
#include "TFC/Core.new.h"

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
struct MemberFunction;

template<typename T, bool = HasCallOperator<T>::Value>
struct CallableObject
{
	typedef std::false_type Callable;
};

template<typename T>
struct CallableObject<T, true> : MemberFunction<decltype(&T::operator())>
{
	typedef std::true_type Callable;
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

}}}

#define EventOf(EVENT_VAR) decltype(TFC::Core::Introspect::EventOfFunc(& EVENT_VAR))


#endif /* INTROSPECT_H_ */
