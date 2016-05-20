/*
 * Sharing.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Util/Sharing.h"
#include "SRIN/Core.h"
#include <app_control.h>

using namespace SRIN::Util;

LIBAPI void SRIN::Util::ShareString(const std::string& str)
{
	app_control_h appControl;

	app_control_create(&appControl);

	app_control_set_operation(appControl, APP_CONTROL_OPERATION_SHARE_TEXT);
	app_control_add_extra_data(appControl, APP_CONTROL_DATA_TEXT, str.c_str());
	app_control_set_launch_mode(appControl, APP_CONTROL_LAUNCH_MODE_GROUP);

	app_control_send_launch_request(appControl, NULL, NULL);

	app_control_destroy(appControl);

}

LIBAPI bool SRIN::Util::CopyStringToClipboard(const std::string& str, Evas_Object* source)
{
	Eina_Bool ret = elm_cnp_selection_set(source, ELM_SEL_TYPE_CLIPBOARD, ELM_SEL_FORMAT_TEXT, str.c_str(), str.size());
	return (ret == EINA_TRUE);
}

LIBAPI void SRIN::Util::OpenURL(const std::string& url)
{
	app_control_h app_control;
	app_control_create(&app_control);

	app_control_set_operation(app_control, APP_CONTROL_OPERATION_VIEW);
	app_control_set_uri(app_control, url.c_str());

	app_control_send_launch_request(app_control, NULL, NULL);
	app_control_destroy(app_control);
}

LIBAPI void SRIN::Util::LaunchViewer(const std::string& uri, const std::string& mimeType)
{
	app_control_h appControl;
	app_control_create(&appControl);

	app_control_set_operation(appControl, APP_CONTROL_OPERATION_VIEW);
	app_control_set_mime(appControl, mimeType.c_str());
	app_control_set_uri(appControl, uri.c_str());

	app_control_send_launch_request(appControl, NULL, NULL);
	app_control_destroy(appControl);
}

LIBAPI void SRIN::Util::ComposeMailForOne(const std::string& recipient, const std::string& subject, const std::string& text)
{
	app_control_h appControl;
	app_control_create(&appControl);

	app_control_set_operation(appControl, APP_CONTROL_OPERATION_COMPOSE);
	app_control_set_uri(appControl, recipient.c_str());
	app_control_add_extra_data(appControl, APP_CONTROL_DATA_SUBJECT, subject.c_str());
	app_control_add_extra_data(appControl, APP_CONTROL_DATA_TEXT, text.c_str());

	app_control_set_launch_mode(appControl, APP_CONTROL_LAUNCH_MODE_GROUP);

	app_control_send_launch_request(appControl, NULL, NULL);
	app_control_destroy(appControl);
}
