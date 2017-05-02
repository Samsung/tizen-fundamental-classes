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
 *    Net/Util.cpp
 *
 * Created on:  Mar 1, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Net/Util.h"

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

std::string TFC::Net::Base64Encode(const uint8_t* buffer, size_t length)
{
	BIO* bio;
	BIO* b64;
	BUF_MEM* bufferPtr;
	int outlen;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
	outlen = BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);

	// Copy data to result
	std::string encoded(bufferPtr->data, bufferPtr->length);

	BIO_free_all(bio);

	return encoded;
}
