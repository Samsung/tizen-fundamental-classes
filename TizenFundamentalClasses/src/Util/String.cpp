/*
 * String.cpp
 *
 *  Created on: Jun 23, 2017
 *      Author: gmh
 */
#include "TFC/Core.h"
#include "TFC/Util/String.h"
#include <cstdarg>
#include <cstring>

LIBAPI
std::string TFC::Util::String::Format(const char* format, ...)
{
	// From:
	// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf


	int size = strlen(format) * 2 + 50;
	std::string str;
	va_list argList;

	while (true)
	{
		str.resize(size);

		va_start(argList, format);
		int n = vsnprintf((char *)str.data(), size, format, argList);
		va_end(argList);

		if (n > -1 && n < size)
		{
			str.resize(n);
			return str;
		}

		// Needs resize
		if (n > -1)
			size = n + 1;
		else
			size *= 2;
	}
	return str;
}
