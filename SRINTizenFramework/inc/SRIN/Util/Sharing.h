/*
 * Sharing.h
 *
 *  Created on: Mar 28, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_SHARING_H_
#define SRIN_SHARING_H_

#include <string>
#include <efl_extension.h>

/**
 * Wrapper for Tizen App Control functions.
 */
namespace SRIN { namespace Util {
	/**
	 * Method to share a text by bluetooth, memo, etc.
	 */
	void ShareString(const std::string& str);

	/**
	 * Method to store text to clipboard.
	 */
	bool CopyStringToClipboard(const std::string& str, Evas_Object* source);

	/**
	 * Method to open native web browser and point it to a particular URL.
	 */
	void OpenURL(const std::string& url);

	/**
	 * Method to launch native multimedia viewer applications, e.g. video player.
	 */
	void LaunchViewer(const std::string& videoURL, const std::string& mimeType);

	/**
	 * Method to compose an email with one recipient.
	 */
	void ComposeMailForOne(const std::string& recipient, const std::string& subject, const std::string& text);

}
}


#endif /* SHARING_H_ */
