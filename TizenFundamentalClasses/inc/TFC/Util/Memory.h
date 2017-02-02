/*
 * Memory.h
 *
 *  Created on: Feb 1, 2017
 *      Author: gilang
 */

#ifndef TFC_UTIL_MEMORY_H_
#define TFC_UTIL_MEMORY_H_

#include <memory>

namespace TFC {
namespace Util {

class FreeDeleter
{
public:
	void operator()(void* ptr)
	{
		if(ptr != nullptr)
			free(ptr);
	}
};

typedef std::unique_ptr<void*, FreeDeleter> MallocPtr;

}}



#endif /* TFC_UTIL_MEMORY_H_ */
