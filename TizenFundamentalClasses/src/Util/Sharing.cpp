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
 *    Util/Sharing.cpp
 *
 * Created on:  Mar 28, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Util/Sharing.h"
#include "TFC/Core.h"
#include <app_control.h>

using namespace TFC::Util;

LIBAPI void TFC::Util::ShareString(const std::string& str)
{
	app_control_h appControl;

	app_control_create(&appControl);

	app_control_set_operation(appControl, APP_CONTROL_OPERATION_SHARE_TEXT);
	app_control_add_extra_data(appControl, APP_CONTROL_DATA_TEXT, str.c_str());
	app_control_set_launch_mode(appControl, APP_CONTROL_LAUNCH_MODE_GROUP);

	app_control_send_launch_request(appControl, NULL, NULL);

	app_control_destroy(appControl);

}

LIBAPI bool TFC::Util::CopyStringToClipboard(const std::string& str, Evas_Object* source)
{
	Eina_Bool ret = elm_cnp_selection_set(source, ELM_SEL_TYPE_CLIPBOARD, ELM_SEL_FORMAT_TEXT, str.c_str(), str.size());
	return (ret == EINA_TRUE);
}

LIBAPI void TFC::Util::OpenURL(const std::string& url)
{
	app_control_h app_control;
	app_control_create(&app_control);

	app_control_set_operation(app_control, APP_CONTROL_OPERATION_VIEW);
	app_control_set_uri(app_control, url.c_str());

	app_control_send_launch_request(app_control, NULL, NULL);
	app_control_destroy(app_control);
}

LIBAPI void TFC::Util::LaunchViewer(const std::string& uri, const std::string& mimeType)
{
	app_control_h appControl;
	app_control_create(&appControl);

	app_control_set_operation(appControl, APP_CONTROL_OPERATION_VIEW);
	app_control_set_mime(appControl, mimeType.c_str());
	app_control_set_uri(appControl, uri.c_str());

	app_control_send_launch_request(appControl, NULL, NULL);
	app_control_destroy(appControl);
}

LIBAPI void TFC::Util::ComposeMailForOne(const std::string& recipient, const std::string& subject, const std::string& text)
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
