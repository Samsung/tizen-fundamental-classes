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
 *    Net/Util.h
 *
 * Utility functions
 *
 * Created on:  Mar 1, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#ifndef TFCFW_UTIL_H_
#define TFCFW_UTIL_H_

#include <string>

namespace TFC {
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
