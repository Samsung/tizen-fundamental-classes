/*
 * FloatingButton.cpp
 *
 *  Created on: May 17, 2016
 *      Contributor:
 *        Kevin Winata (k.winata@samsung.com)
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Components/FloatingButton.h"
#include "TFC/Framework/Application.h"
#include <efl_extension.h>

using namespace TFC::Components;

LIBAPI FloatingButton::FloatingButton() :
		naviframe(nullptr),
		floatingButton(nullptr),
		buttonLeft(nullptr),
		buttonRight(nullptr),
		buttonLeftImage(""),
		buttonRightImage(""),
		doubleButton(false),
		movementBlock(false)
{

}

LIBAPI Evas_Object* FloatingButton::CreateComponent(Evas_Object* root)
{
	naviframe = root;

	floatingButton = eext_floatingbutton_add(naviframe);
	elm_object_part_content_set(naviframe, "elm.swallow.floatingbutton", floatingButton);

	buttonLeft = elm_button_add(floatingButton);
	elm_object_part_content_set(floatingButton, "button1", buttonLeft);
	//evas_object_smart_callback_add(buttonLeft, "clicked", &EFL::EvasSmartEventHandler, &eventButtonLeftClick);
	eventButtonLeftClick.Bind(buttonLeft, "clicked");

	if (buttonLeftImage.size() > 0) {
		auto image = elm_image_add(floatingButton);
		std::string path = TFC::Framework::ApplicationBase::GetResourcePath(buttonLeftImage.c_str());
		elm_image_file_set(image, path.c_str(), NULL);
		elm_object_part_content_set(buttonLeft, "icon", image);
	}

	if (doubleButton) {
		buttonRight = elm_button_add(floatingButton);
		elm_object_part_content_set(floatingButton, "button2", buttonRight);
		//evas_object_smart_callback_add(buttonRight, "clicked", &EFL::EvasSmartEventHandler, &eventButtonRightClick);
		eventButtonRightClick.Bind(buttonRight, "clicked");

		if (buttonRightImage.size() > 0) {
			auto image = elm_image_add(floatingButton);
			std::string path = TFC::Framework::ApplicationBase::GetResourcePath(buttonRightImage.c_str());
			elm_image_file_set(image, path.c_str(), NULL);
			elm_object_part_content_set(buttonRight, "icon", image);
		}
	}

	if (movementBlock) {
		eext_floatingbutton_movement_block_set(floatingButton, EINA_TRUE);
	}

	evas_object_show(floatingButton);

	return floatingButton;
}

LIBAPI void FloatingButton::SetWhiteBackground()
{
	elm_object_style_set(floatingButton, "white_bg");
}

LIBAPI void FloatingButton::SetButtonStyle(bool left, const std::string& style)
{
	auto button = (left) ? buttonLeft : buttonRight;
	elm_object_style_set(button, style.c_str());
}
