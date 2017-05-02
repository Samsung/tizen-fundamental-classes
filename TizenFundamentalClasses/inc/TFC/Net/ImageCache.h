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
 *    Net/ImageCache.h
 *
 * Helper functions to perform image caching from online image source
 *
 * Created on: 	Mar 14, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#ifndef IMAGECACHE_H_
#define IMAGECACHE_H_

#include "TFC/Core.h"
#include <string>

namespace TFC {
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
