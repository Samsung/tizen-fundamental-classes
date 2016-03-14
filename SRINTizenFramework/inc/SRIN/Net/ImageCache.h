/*
 * ImageCache.h
 *
 *  Created on: Mar 14, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef IMAGECACHE_H_
#define IMAGECACHE_H_

#include "SRIN/Core.h"
#include <string>

namespace SRIN {
namespace Net {
namespace ImageCache {

std::string LoadImage(const std::string& url);
bool RequireDownloading(const std::string& url, std::string& filePath);

}
}
}



#endif /* IMAGECACHE_H_ */
