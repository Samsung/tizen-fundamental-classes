/*
 * Invocation.h
 *
 *  Created on: Jan 21, 2017
 *      Author: gilang
 */

#ifndef TFC_CORE_INVOCATION_H_
#define TFC_CORE_INVOCATION_H_

#include "TFC/Core.h"
#include "TFC/Core/Introspect.h"
#include "TFC/Core/Metaprogramming.h"

namespace TFC {
namespace Core {



std::string GetInterfaceName(char const* prefix, std::type_info const& i);

template<typename TFunctionType,
		 typename TParameterPack = typename Core::Introspect::MemberFunction<TFunctionType>::ArgsTuple>
struct DelayedInvoker;

template<typename TFunctionType, typename... TArgs>
struct DelayedInvoker<TFunctionType, std::tuple<TArgs...>>
{
	typedef typename Core::Introspect::MemberFunction<TFunctionType>::DeclaringType InstanceType;
	typedef typename Core::Introspect::MemberFunction<TFunctionType>::ReturnType ReturnType;
	typedef typename Core::Metaprogramming::SequenceGenerator<sizeof...(TArgs)>::Type ArgSequence;

	template<int... S>
	static ReturnType Call(InstanceType* i, TFunctionType ptr, std::tuple<TArgs...> const& param, Core::Metaprogramming::Sequence<S...>)
	{
		return (i->*ptr)(std::get<S>(param)...);
	}

	static ReturnType Invoke(InstanceType* i, TFunctionType ptr, std::tuple<TArgs...> const& args)
	{
		return Call(i, ptr, args, ArgSequence());
	}
};

template<typename TClass,
		 typename TArgsTuple>
struct DelayedConstructor;

template<typename TClass, typename... TArgs>
struct DelayedConstructor<TClass, std::tuple<TArgs...>>
{
	typedef typename Core::Metaprogramming::SequenceGenerator<sizeof...(TArgs)>::Type ArgSequence;

	template<int... S>
	static TClass* Call(std::tuple<TArgs...> const& param, Core::Metaprogramming::Sequence<S...>)
	{
		return new TClass(std::get<S>(param)...);
	}

	static TClass* Invoke(std::tuple<TArgs...> const& args)
	{
		return Call(args, ArgSequence());
	}

	template<int... S>
	static TClass* CallThrow(std::tuple<TArgs...> const& param, Core::Metaprogramming::Sequence<S...>)
	{
		throw TClass(std::get<S>(param)...);
	}

	static void Throw(std::tuple<TArgs...> const& args)
	{
		CallThrow(args, ArgSequence());
	}
};

}}



#endif /* TFC_CORE_INVOCATION_H_ */
