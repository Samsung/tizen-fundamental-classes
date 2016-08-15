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

/**
 * Method to download an image.
 *
 * @param url URL that contains an image file.
 *
 * @return full path to the image if download is successful, the URL otherwise as a fallback.
 *
 * @note To ensure that no duplicate image is downloaded, we can call {RequireDownloading} before
 * this method.
 */
std::string LoadImage(const std::string& url);

/**
 * Method to check whether a particular image URL has been downloaded before.
 * If not, this method will generate an unique filename and database entry, then return true.
 * This way, when the method is called again with URL parameter that already exist in database,
 * then it will update filePath parameter with the filename from the database.
 *
 * @param url URL that contains an image file.
 * @param filePath String reference that will be updated with the generated filename.
 *
 * @return true if the url is never been downloaded, false otherwise
 */
bool RequireDownloading(const std::string& url, std::string& filePath);

}
}
}



#endif /* IMAGECACHE_H_ */
