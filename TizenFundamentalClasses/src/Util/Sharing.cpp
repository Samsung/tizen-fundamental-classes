/*
 * Sharing.cpp
 *
 *  Created on: Mar 28, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Util/Sharing.h"
#include "TFC/Core.h"
#include <app_control.h>
#include <media_content.h>
#include <dlog.h>
#include <sys/stat.h>
#include <fstream>

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

LIBAPI void TFC::Util::ShareData(const std::string& text, const std::string& imagePathOri) {
    dlog_print(DLOG_ERROR, LOG_TAG, "text = %s path = %s", text.c_str() , imagePathOri.c_str());

    std::string imagePath(imagePathOri);
    if(imagePath.empty()) {
    	imagePath.append("/opt/usr/apps/com.srin.indramayu.tizen/res/image/Blank_Image_News_Revisi.png");
    	dlog_print(DLOG_ERROR, LOG_TAG, "text 2 = %s path 2 = %s", text.c_str() , imagePath.c_str());
    }

	//get image filename
	std::size_t found = imagePath.find_last_of("/\\");
	std::string filename = "/opt/usr/media/Images/ggi/";
	filename.append(imagePath.substr(found+1));

	media_info_h mediaInfo = NULL;
    mkdir("/opt/usr/media/Images/ggi/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    std::ifstream  src(imagePath, std::ios::binary);
    std::ofstream  dst(filename,   std::ios::binary);
    dst << src.rdbuf();
    dst.close();
    src.close();

    dlog_print(DLOG_ERROR, LOG_TAG, "filename = %s", filename.c_str());

    media_content_connect();
    int ret = media_info_insert_to_db(filename.c_str(), &mediaInfo);
    if ((MEDIA_CONTENT_ERROR_NONE == ret) && (NULL != mediaInfo))
        dlog_print(DLOG_ERROR, LOG_TAG, "Insertion succeed, ret: %d", ret);
    else {
        dlog_print(DLOG_ERROR, LOG_TAG, "Insertion failed, ret: %d", ret);
    }

	app_control_h appControl;

	app_control_create(&appControl);

	app_control_set_operation(appControl, APP_CONTROL_OPERATION_SHARE);
	app_control_set_uri(appControl, filename.c_str());
	app_control_set_mime(appControl, "image/*");
	app_control_add_extra_data(appControl, APP_CONTROL_DATA_PATH, filename.c_str());
//	app_control_add_extra_data(appControl, APP_CONTROL_DATA_SUBJECT, "TEST SUBJECT");
//	app_control_add_extra_data(appControl, APP_CONTROL_DATA_TITLE, "TEST TITLE");
	app_control_add_extra_data(appControl, APP_CONTROL_DATA_TEXT, text.c_str());
	//app_control_add_extra_data(appControl, APP_CONTROL_DATA_TEXT, str.c_str());
	app_control_set_launch_mode(appControl, APP_CONTROL_LAUNCH_MODE_GROUP);

	app_control_send_launch_request(appControl, NULL, NULL);

	media_info_destroy(mediaInfo);
	app_control_destroy(appControl);
}
