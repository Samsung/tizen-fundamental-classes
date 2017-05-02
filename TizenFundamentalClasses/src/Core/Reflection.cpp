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
 *    Core/Reflection.cpp
 *
 * Created on:  Jan 21, 2017
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */


#include "TFC/Core/Reflection.h"
#include <unordered_map>
#include <cxxabi.h>


using namespace TFC;
using namespace TFC::Core;

std::unordered_map<std::string, TypeDescription&> descriptionTable;

LIBAPI
TypeDescription::TypeDescription(std::initializer_list<TypeDescriptionBuilder>& init, std::type_info const& info)
{
	destructor = nullptr;
	for(auto& i : init)
	{
		switch(i.kind)
		{
		case TypeMemberKind::Function:
			{
				auto funcInfo = dynamic_cast<FunctionInfo*>(i.infoObject);
				functionMap.emplace(std::make_pair(funcInfo->functionName, funcInfo));
			}
			break;
		case TypeMemberKind::Constructor:
			{
				auto consInfo = dynamic_cast<ConstructorInfo*>(i.infoObject);
				constructorMap.emplace(std::make_pair(consInfo->constructorName, consInfo));
			}
			break;
		case TypeMemberKind::Destructor:
			{
				destructor = dynamic_cast<DestructorInfo*>(i.infoObject);
			}
			break;
		case TypeMemberKind::Event:
			{
				auto eventInfo = dynamic_cast<EventInfo*>(i.infoObject);
				eventMap.emplace(std::make_pair(eventInfo->eventName, eventInfo));
				dlog_print(DLOG_DEBUG, "RPC-Test", "Event info registration completed");
			}
			break;
		}
	}

	descriptionTable.emplace(std::pair<std::string, TypeDescription&>(std::string(info.name()), *this));
}

LIBAPI
TypeDescription const& TFC::Core::FindTypeByName(const std::string& name)
{
	auto typeIter = descriptionTable.find(name);

	if(typeIter == descriptionTable.end())
		throw TFC::Core::TypeNotFoundException("Typename not registered");

	return typeIter->second;
}
