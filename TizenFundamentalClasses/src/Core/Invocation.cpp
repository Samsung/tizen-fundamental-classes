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
 *    Core/Invocation.cpp
 *
 * Created on:  Dec 27, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Core.h"
#include "TFC/Core/Invocation.h"

#include <regex>
#include <cxxabi.h>

LIBAPI
std::string TFC::Core::GetInterfaceName(char const* prefix, std::type_info const& i)
{
	int status = 0;
	auto realName = abi::__cxa_demangle(i.name(), nullptr, nullptr, &status);

	if(status != 0) throw TFC::Core::InvocationException("Failed getting interface name"); // Change exception

	std::string ret;

	if(prefix != nullptr)
	{
		ret = prefix;
		ret.append(".");
	}

	ret.append(realName);
	ret = std::regex_replace(ret, std::regex("::"), ".");

	free(realName);

	return ret;
}
