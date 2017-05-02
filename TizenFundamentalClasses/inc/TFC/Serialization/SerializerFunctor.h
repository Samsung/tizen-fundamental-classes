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
 *    Serialization/SerializerFunctor.h
 *
 * Functor used in metaprogramming for generic serialization processes
 *
 * Created on:  Jan 18, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef TFC_SERIALIZATION_H_
#error "Cannot include SerializerFunctor.h by itself without including TFC/Serialization.h"
#else

#ifndef TFC_SERIALIZATION_SERIALIZERFUNCTOR_H_
#define TFC_SERIALIZATION_SERIALIZERFUNCTOR_H_

namespace TFC {
namespace Serialization {

template<typename TSerializerClass, typename TTarget>
struct SerializerExist
{
	typedef char Correct;
	typedef struct { char val[2]; } Incorrect;

	template<typename TA, typename TB> static Correct Test(decltype(std::declval<TA>().Serialize(std::declval<TB>()))*);
	template<typename TA, typename TB> static Incorrect Test(...);

	public:
		static constexpr bool Value = sizeof(Test<TSerializerClass, TTarget>(0)) == sizeof(Correct);
};

template<typename TSerializerClass, typename TCurrent, typename = void, bool = SerializerExist<TSerializerClass, TCurrent>::Value>
struct SerializerSelect
{
	static void Serialize(TSerializerClass& p, TCurrent t)
	{
		p.Serialize(t);
	}
};

template<typename TSerializerClass, typename TCurrent>
struct SerializerSelect<TSerializerClass, TCurrent, typename std::enable_if<std::is_enum<TCurrent>::value>::type, false>
{
	static void Serialize(TSerializerClass& p, TCurrent t)
	{
		typedef typename std::underlying_type<TCurrent>::type CastedType;
		p.Serialize(static_cast<CastedType>(t));
	}
};

template<typename TSerializerClass, typename TCurrent, typename TVoid>
struct SerializerSelect<TSerializerClass, TCurrent, TVoid, false>
{
	static void Serialize(TSerializerClass& p, TCurrent t)
	{
		TSerializerClass ip = p.CreateScope();
		GenericSerializer<TSerializerClass, TCurrent>::Serialize(ip, t);
		p.Serialize(ip);
	}
};

template<typename T>
struct SerializerField
{
	T const& value;
	bool serialize;
};

/**
 * Recursion case for SerializerFunctor
 */
template<typename TSerializerClass, typename TCurrent, typename... TArgs>
struct SerializerFunctor<TSerializerClass, TCurrent, TArgs...>
{

	static void Func(TSerializerClass& p, TCurrent t, TArgs... next)
	{
		SerializerSelect<TSerializerClass, typename std::decay<TCurrent>::type>::Serialize(p, t);
		// Call SerializerFunctor recursive by passing the TArgs tails as arguments
		SerializerFunctor<TSerializerClass, TArgs...>::Func(p, next...);
	}


	static void Func(TSerializerClass& p, SerializerField<TCurrent> t, SerializerField<TArgs>... next)
	{
		if(t.serialize)
		{
			SerializerSelect<TSerializerClass, TCurrent>::Serialize(p, t.value);
		}

		// Call SerializerFunctor recursive by passing the TArgs tails as arguments
		SerializerFunctor<TSerializerClass, TArgs...>::Func(p, next...);
	}
};



/**
 * Base case for SerializerFunctor where TArgs is nil entirely
 */
template<typename TSerializerClass>
struct SerializerFunctor<TSerializerClass>
{
	// Base function just do nothing
	static void Func(TSerializerClass& p) { }
};

}}



#endif /* TFC_SERIALIZATION_SERIALIZERFUNCTOR_H_ */
#endif
