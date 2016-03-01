/*
 * Util.h
 *
 *  Created on: Mar 1, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRINFW_UTIL_H_
#define SRINFW_UTIL_H_

#include <string>

namespace SRIN { namespace Net {
	std::string Base64Encode(const uint8_t* buffer, size_t length);
}}



#endif /* UTIL_H_ */
