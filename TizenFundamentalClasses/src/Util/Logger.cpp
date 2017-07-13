/*
 * Util.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: gmh
 */

#include "TFC/Util/Logger.h"

#include <cstdarg>
#include <dlog.h>
#include <unistd.h>
#include <fcntl.h>
#include "TFC/Core.h"

using namespace TFC::Util;

namespace {
	char const* logTypeStr[] = {
		"",
		"",
		"VERBOSE",
		"DEBUG",
		"INFO",
		"WARNING",
		"ERROR",
		"FATAL"
	};

	std::ostream* ostream = &std::cerr;

	int32_t logLevel = Logger::Debug;

	void StreamLogger(int32_t mode, char const* tag, char const* text, va_list args)
	{

		int size = strlen(text) * 2 + 50;
		std::string str;

		while (true)
		{
			str.resize(size);

			int n = vsnprintf((char *)str.data(), size, text, args);

			if (n > -1 && n < size)
			{
				str.resize(n);
				break;
			}

			// Needs resize
			if (n > -1)
				size = n + 1;
			else
				size *= 2;
		}

		std::cerr << '[' << logTypeStr[mode] << "] ";

		if(tag != nullptr && *tag != '\0')
		{
			std::cerr << '[' << tag << "] ";
		}

		std::cerr << str << std::endl;
	}

	void DlogLogger(int32_t mode, char const* tag, char const* text, va_list args)
	{
		dlog_vprint((log_priority)mode, tag, text, args);
	}

	void (*loggerFunc)(int32_t mode, char const* tag, char const* text, va_list args) = DlogLogger;
}

LIBAPI
void TFC::Util::Logger::UseDLog()
{
	ostream = nullptr;
	loggerFunc = DlogLogger;
}

LIBAPI
void TFC::Util::Logger::LogLevel(int32_t level)
{
	TFC::TFCAssert(level >= Verbose && level <= Fatal, "Invalid log level set in Logger");
	::logLevel = level;
}

LIBAPI
void TFC::Util::Logger::AttachFile(const std::string& fileName)
{
	ostream = &std::cerr;
	// Redirect std::cerr to new file
	dup2(creat(fileName.c_str(), 0600), 2);
	loggerFunc = StreamLogger;
}

LIBAPI
void TFC::Util::Logger::Log(int32_t type, const char* tag, const char* text, ...)
{
	if(type < logLevel)
		return;

	va_list argList;
	va_start(argList, text);
	(*loggerFunc)(type, tag, text, argList);
	va_end(argList);
}
