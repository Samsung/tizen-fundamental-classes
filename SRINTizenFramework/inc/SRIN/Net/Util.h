/*
 * Util.h
 *
 *  Created on: Mar 1, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRINFW_UTIL_H_
#define SRINFW_UTIL_H_

#include <string>

namespace SRIN {
namespace Net {
/**
 * Static method do base64 encoding of a string.
 * It uses OpenSSL's BIO_f_base64() filter method.
 *
 * @param buffer String that will be encoded.
 * @param length Length of the string.
 *
 * @return Base64 encoded string.
 */
std::string Base64Encode(const uint8_t* buffer, size_t length);
}
}

#endif /* UTIL_H_ */
