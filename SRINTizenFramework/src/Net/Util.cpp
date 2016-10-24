/*
 * Util.cpp
 *
 *  Created on: Mar 1, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
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
