/*
 * Metaprogramming.h
 *
 *  Created on: Dec 28, 2016
 *      Author: gilang
 */

#ifndef TFC_CORE_METAPROGRAMMING_H_
#define TFC_CORE_METAPROGRAMMING_H_

namespace TFC {
namespace Core {
namespace Metaprogramming {

template<int...>
struct Sequence {};

template<int N, int... S>
struct SequenceGenerator : SequenceGenerator<N-1, N-1, S...> {};

template<int... S>
struct SequenceGenerator<0, S...>
{
	typedef Sequence<S...> Type;
};

}}}



#endif /* TFC_CORE_METAPROGRAMMING_H_ */
