/*
 * InterfaceInspector.h
 *
 *  Created on: Dec 21, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_
#define TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_


#include <tuple>

#include <dlog.h>

#include "TFC/Core/Introspect.h"
#include "TFC/Core/Metaprogramming.h"

namespace TFC {
namespace ServiceModel {

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
		return std::move((i->*ptr)(std::get<S>(param)...));
	}

	static ReturnType Invoke(InstanceType* i, TFunctionType ptr, std::tuple<TArgs...> const& args)
	{
		return std::move(Call(i, ptr, args, ArgSequence()));
	}
};

}}




#define TFC_DiscriminatedUnionInfo(MEMPTR, DISCR, ... ) TFC::ServiceModel::DiscriminatedUnionFieldInfo< typename TFC::Core::Introspect::MemberField< decltype( & MEMPTR ) >::DeclaringType, decltype( MEMPTR ), & MEMPTR, decltype( DISCR ), & DISCR, __VA_ARGS__ >



#endif /* TFC_SERVICEMODEL_INTERFACEINSPECTOR_H_ */
