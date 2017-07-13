/*
 * Logger.h
 *
 *  Created on: Jul 11, 2017
 *      Author: gmh
 */

#ifndef TFC_UTIL_LOGGER_H_
#define TFC_UTIL_LOGGER_H_

#include <iostream>
namespace TFC {
namespace Util {


	class Logger
	{
	public:
		enum
		{
			Verbose = 2,
			Debug,
			Info,
			Warning,
			Error,
			Fatal
		};

		static void UseDLog();
		static void AttachFile(std::string const& fileName);

		static void LogLevel(int32_t level);

		static void Log(int32_t type, char const* tag, char const* text, ...);

		template<typename... TArgs>
		static void Log(int32_t type, std::string const& tag, std::string const& text, typename std::enable_if<std::is_trivial<TArgs>::value, TArgs>::type... param)
		{
			Log(type, tag.c_str(), text.c_str(), std::forward<TArgs>(param)...);
		}


	};

}
}

#define TFC_Log TFC::Util::Logger::Log

#endif /* TFC_UTIL_LOGGER_H_ */
