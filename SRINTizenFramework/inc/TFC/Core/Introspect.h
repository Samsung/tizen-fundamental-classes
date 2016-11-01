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



}}}

#define EventOf(EVENT_VAR) decltype(TFC::Core::Introspect::EventOfFunc(& EVENT_VAR))


#endif /* INTROSPECT_H_ */
