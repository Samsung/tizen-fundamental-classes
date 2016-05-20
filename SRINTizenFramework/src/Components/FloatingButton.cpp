/*
 * FloatingButton.cpp
 *
 *  Created on: May 17, 2016
 *      Author: Kevin Winata
 */

#include "SRIN/Components/FloatingButton.h"
#include "SRIN/Framework/Application.h"
#include <efl_extension.h>

using namespace SRIN::Components;

LIBAPI FloatingButton::FloatingButton() :
		naviframe(nullptr),
		floatingButton(nullptr),
		buttonLeft(nullptr),
		buttonRight(nullptr),
		buttonLeftImage(""),
		buttonRightImage(""),
		doubleButton(false)
{

}

LIBAPI Evas_Object* FloatingButton::CreateComponent(Evas_Object* root)
{
	naviframe = root;

	floatingButton = eext_floatingbutton_add(naviframe);
	elm_object_part_content_set(naviframe, "elm.swallow.floatingbutton", floatingButton);

	buttonLeft = elm_button_add(floatingButton);
	elm_object_part_content_set(floatingButton, "button1", buttonLeft);
	evas_object_smart_callback_add(buttonLeft, "clicked", &SmartEventHandler, &eventButtonLeftClick);

	if (buttonLeftImage.size() > 0) {
		auto image = elm_image_add(floatingButton);
		std::string path = SRIN::Framework::ApplicationBase::GetResourcePath(buttonLeftImage.c_str());
		elm_image_file_set(image, path.c_str(), NULL);
		elm_object_part_content_set(buttonLeft, "icon", image);
	}

	if (doubleButton) {
		buttonRight = elm_button_add(floatingButton);
		elm_object_part_content_set(floatingButton, "button2", buttonRight);
		evas_object_smart_callback_add(buttonRight, "clicked", &SmartEventHandler, &eventButtonRightClick);

		if (buttonRightImage.size() > 0) {
			auto image = elm_image_add(floatingButton);
			std::string path = SRIN::Framework::ApplicationBase::GetResourcePath(buttonRightImage.c_str());
			elm_image_file_set(image, path.c_str(), NULL);
			elm_object_part_content_set(buttonRight, "icon", image);
		}
	}

	evas_object_show(floatingButton);

	return floatingButton;
}
