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
 *    Core/Invocation.h
 *
 * Templates to help performing invocation of other function in runtime
 * by providing correct tuple of the parameter. This header is to support
 * experimental runtime reflection in C++.
 *
 * Created on: 	Jan 23, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
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
