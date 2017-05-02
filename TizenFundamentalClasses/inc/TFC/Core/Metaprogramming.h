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
 *    Core/Metaprogramming.h
 *
 * General purpose metaprogram classes which is not provided by C++
 * standard library but required by TFC
 *
 * Created on: 	Oct 25, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
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

template<typename... Ts> struct MakeVoid { typedef void Type;};
template<typename... Ts> using Void_T = typename MakeVoid<Ts...>::Type;

}}}



#endif /* TFC_CORE_METAPROGRAMMING_H_ */
