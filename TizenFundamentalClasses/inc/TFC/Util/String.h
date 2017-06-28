/*
 * String.h
 *
 *  Created on: Jun 23, 2017
 *      Author: gmh
 */

#ifndef TFC_UTIL_STRING_H_
#define TFC_UTIL_STRING_H_

#include <string>

namespace TFC {
namespace Util {
namespace String {

	std::string Format(char const* format, ...);
	std::string Format(std::string const& format, ...);

}}}

#define TFC_Format(...) TFC::Util::String::Format( __VA_ARGS__ )


#endif /* TFC_UTIL_STRING_H_ */
