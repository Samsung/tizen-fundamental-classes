/*
 * Predicates.h
 *
 *  Created on: Jan 18, 2017
 *      Author: gilang
 */

#ifndef TFC_SERIALIZATION_PREDICATES_H_
#define TFC_SERIALIZATION_PREDICATES_H_

#include "TFC/Core/Introspect.h"

namespace TFC {
namespace Serialization {
namespace Predicates {

template<typename T, typename TValue, TValue T::* path, TValue v>
struct ValueCheckPredicate
{
	static bool Evaluate(T const& obj)
	{
		return obj.*path == v;
	}
};

template<typename T, typename TValue, TValue T::* path, uint32_t v>
struct BitFieldCheckPredicate
{
	static bool Evaluate(T const& obj)
	{
		uint32_t val = (uint32_t)(obj.*path);
		return (val & v) != 0;
	}
};

template<typename TPredicate>
struct NotPredicate
{
	template<typename T>
	static bool Evaluate(T const& obj)
	{
		return !TPredicate::Evaluate(obj);
	}
};

}}}

#define TFC_BitFieldCheck(MEMPTR, VAL) TFC::Serialization::Predicates::BitFieldCheckPredicate< typename TFC::Core::Introspect::MemberField< decltype( & MEMPTR ) >::DeclaringType, typename TFC::Core::Introspect::MemberField< decltype( & MEMPTR ) >::ValueType, & MEMPTR, VAL >



#endif /* TFC_SERIALIZATION_PREDICATES_H_ */
