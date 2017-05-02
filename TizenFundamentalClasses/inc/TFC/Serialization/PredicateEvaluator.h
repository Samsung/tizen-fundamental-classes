/*
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
 *    Serialization/PredicateEvaluator.h
 *
 * Evaluator for declared Predicate macros
 *
 * Created on:  Jan 18, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef TFC_SERIALIZATION_PREDICATEEVALUATOR_H_
#define TFC_SERIALIZATION_PREDICATEEVALUATOR_H_

#include "TFC/Serialization.h"

namespace TFC {
namespace Serialization {

template<typename... TPredicates>
struct PredicateEvaluator<std::tuple<TPredicates...>>
{

	template<typename TPredicate, typename T>
	static bool CallEvaluate(T const& obj)
	{
		return TPredicate::Evaluate(obj);
	}

	template<typename T>
	static bool Evaluate(T const& obj)
	{
		bool res = true;
		using Expansion  = int[];
		(void)Expansion {0, ((res = res && CallEvaluate<TPredicates>(obj)), 0)... };

		return res;
	}
};

template<>
struct PredicateEvaluator<std::tuple<>>
{
	template<typename T>
	static bool Evaluate(T const& obj)
	{
		return true;
	}
};



}}



#endif /* TFC_SERIALIZATION_PREDICATEEVALUATOR_H_ */
