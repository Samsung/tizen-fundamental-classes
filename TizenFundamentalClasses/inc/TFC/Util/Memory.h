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
 *    Util/Memory.h
 *
 * Helper classes which wraps STL's memory classes
 *
 * Created on:  Feb 02, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#ifndef TFC_UTIL_MEMORY_H_
#define TFC_UTIL_MEMORY_H_

#include <memory>

namespace TFC {
namespace Util {

class FreeDeleter
{
public:
	void operator()(void* ptr)
	{
		if(ptr != nullptr)
			free(ptr);
	}
};

typedef std::unique_ptr<void*, FreeDeleter> MallocPtr;

}}



#endif /* TFC_UTIL_MEMORY_H_ */
