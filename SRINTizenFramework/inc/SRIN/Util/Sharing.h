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
	 *
	 * @param str String reference that will be shared.
	 */
	void ShareString(const std::string& str);

	/**
	 * Method to store text to clipboard.
	 *
	 * @param str String reference that will be copied to clipboard.
	 * @param source Evas object that holds the text.
	 *
	 * @return True if the operation succeeded, false if failed.
	 */
	bool CopyStringToClipboard(const std::string& str, Evas_Object* source);

	/**
	 * Method to open native web browser and point it to a particular URL.
	 *
	 * @param url Address that will be opened in the browser.
	 */
	void OpenURL(const std::string& url);

	/**
	 * Method to launch native multimedia viewer applications, e.g. video player.
	 *
	 * @param uri Address or path of the multimedia file.
	 * @param mimeType MIME type of the file, will affect which viewer application will be called.
	 */
	void LaunchViewer(const std::string& uri, const std::string& mimeType);

	/**
	 * Method to compose an email with one recipient.
	 *
	 * @param recipient Email address of the recipient.
	 * @param subject Subject of the email.
	 * @param text Body of the email.
	 */
	void ComposeMailForOne(const std::string& recipient, const std::string& subject, const std::string& text);

}
}


#endif /* SHARING_H_ */
