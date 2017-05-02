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
 *    Serialization/Predicates.h
 *
 * Macros for serialization predicates to control serialization process flow
 *
 * Created on:  Jan 18, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
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
