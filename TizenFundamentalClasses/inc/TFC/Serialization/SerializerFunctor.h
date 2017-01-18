/*
 * SerializerFunctor.h
 *
 *  Created on: Jan 18, 2017
 *      Author: gilang
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
		TSerializerClass ip;
		ClassSerializer<TSerializerClass, TCurrent>::Serialize(ip, t);
		auto v = ip.EndPack();
		p.Serialize(v);
	}
};

/**
 * Recursion case for SerializerFunctor
 */
template<typename TSerializerClass, typename TCurrent, typename... TArgs>
struct SerializerFunctor<TSerializerClass, TCurrent, TArgs...>
{
	static void Func(TSerializerClass& p, TCurrent t, TArgs... next)
	{
		SerializerSelect<TSerializerClass, TCurrent>::Serialize(p, t);
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
