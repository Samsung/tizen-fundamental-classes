/*
 * ClientEndpoint.cpp
 *
 *  Created on: Dec 27, 2016
 *      Author: gilang
 */

#include "TFC/ServiceModel/ClientEndpoint.h"
#include <regex>
#include <cxxabi.h>

LIBAPI
std::string TFC::ServiceModel::GetInterfaceName(char const* prefix, std::type_info const& i)
{
	int status = 0;
	auto realName = abi::__cxa_demangle(i.name(), nullptr, nullptr, &status);

	if(status != 0) throw std::exception(); // Change exception

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
