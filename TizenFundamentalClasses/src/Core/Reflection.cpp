/*
 * Reflection.cpp
 *
 *  Created on: Jan 21, 2017
 *      Author: gilang
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
