/*
 * PredicateEvaluator.h
 *
 *  Created on: Jan 18, 2017
 *      Author: gilang
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
