/*
 * Sharing.h
 *
 *  Created on: Mar 28, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_SHARING_H_
#define SRIN_SHARING_H_

#include <string>

namespace SRIN { namespace Util {

	void ShareString(const std::string& str);

	void OpenURL(const std::string& url);

	void LaunchViewer(const std::string& videoURL, const std::string& mimeType);

	void ComposeMailForOne(const std::string& recipient, const std::string& subject, const std::string& text);

}
}


#endif /* SHARING_H_ */
