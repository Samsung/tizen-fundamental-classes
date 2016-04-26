/*
 * PopupBox.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: Ida Bagus Putu Peradnya Dinata
 */

#include "SRIN/Components/PopupBox.h"
#include <string>

using namespace SRIN::Components;

LIBAPI Evas_Object* SRIN::Components::PopupBox::CreateComponent(Evas_Object* root) {
	this->root = root;
	return nullptr;
}

LIBAPI Evas_Object* SRIN::Components::PopupBox::CreateContent(Evas_Object* root) {
	return nullptr;
}

LIBAPI bool SRIN::Components::PopupBox::BackButtonPressed() {
	Dismiss();
	return false;
}

LIBAPI void SRIN::Components::PopupBox::SetTitle(const std::string& text) {
	this->title = text;
	elm_object_part_text_set(this->popup, "title,text", this->title.c_str());
}

LIBAPI std::string& SRIN::Components::PopupBox::GetTitle() {
	return this->title;
}

LIBAPI void SRIN::Components::PopupBox::SetMessage(const std::string& text) {
	this->message = text;
}

LIBAPI std::string& SRIN::Components::PopupBox::GetMessageString() {
	return this->message;
}

LIBAPI SRIN::Components::PopupBox::PopupBox() : Message(this), Title(this)
{

	this->popup 		= nullptr;
	this->popupLayout 	= nullptr;
	this->root 			= nullptr;
	this->orientation   = Elm_Popup_Orient::ELM_POPUP_ORIENT_BOTTOM;

	this->Visible 		= false;
	this->title 		= "";
	this->message 		= "";

	buttonOneText 		= "";
	buttonTwoText 		= "";
	buttonThreeText 	= "";

	buttonOneImage 		= "";
	buttonTwoImage 		= "";
	buttonThreeImage 	= "";
}

LIBAPI void SRIN::Components::PopupBox::Show() {
	this->popup = elm_popup_add(root);
	elm_object_part_text_set(this->popup, "title,text", this->title.c_str());
	elm_popup_orient_set(this->popup, this->orientation);

	this->popupLayout = CreateContent(this->root);

	if(message.size() > 0)
		elm_object_text_set(popup, message.c_str());

	if(popupLayout)
		elm_object_content_set(this->popup, this->popupLayout);

	Evas_Object* btn = nullptr;

	if(buttonOneText.size() > 0 || buttonOneImage.size() > 0)
	{
		btn = elm_button_add(popup);
		elm_object_style_set(btn, "popup");
		elm_object_text_set(btn, buttonOneText.c_str());
		elm_object_part_content_set(popup, "button1", btn);
		evas_object_smart_callback_add(btn, "clicked", &SmartEventHandler, &buttonOneClick);

		if(buttonOneImage.size() > 0)
		{
			Evas_Object* img = elm_icon_add(btn);
			std::string path = SRIN::Framework::ApplicationBase::GetResourcePath(buttonOneImage.c_str());
			elm_image_file_set(img, path.c_str(), nullptr);
			evas_object_show(img);

			evas_object_size_hint_min_set(img, ELM_SCALE_SIZE(150), ELM_SCALE_SIZE(150));
			elm_object_part_content_set(btn, "icon", img);
		}
	}

	if(buttonTwoText.size() > 0 || buttonTwoImage.size() > 0)
	{
		btn = elm_button_add(popup);
		elm_object_style_set(btn, "popup");
		elm_object_text_set(btn, buttonTwoText.c_str());
		elm_object_part_content_set(popup, "button2", btn);
		evas_object_smart_callback_add(btn, "clicked", &SmartEventHandler, &buttonTwoClick);

		if(buttonTwoImage.size() > 0)
		{
			Evas_Object* img = elm_icon_add(btn);
			std::string path = SRIN::Framework::ApplicationBase::GetResourcePath(buttonTwoImage.c_str());
			elm_image_file_set(img, path.c_str(), nullptr);
			evas_object_show(img);

			evas_object_size_hint_min_set(img, ELM_SCALE_SIZE(150), ELM_SCALE_SIZE(150));
			elm_object_part_content_set(btn, "icon", img);
		}

		elm_config_scale_get();
	}

	if(buttonThreeText.size() > 0 || buttonThreeImage.size() > 0)
	{
		btn = elm_button_add(popup);
		elm_object_style_set(btn, "popup");
		elm_object_text_set(btn, buttonThreeText.c_str());
		elm_object_part_content_set(popup, "button3", btn);
		evas_object_smart_callback_add(btn, "clicked", &SmartEventHandler, &buttonThreeClick);

		if(buttonThreeImage.size() > 0)
		{
			Evas_Object* img = elm_icon_add(btn);
			std::string path = SRIN::Framework::ApplicationBase::GetResourcePath(buttonThreeImage.c_str());
			elm_image_file_set(img, path.c_str(), nullptr);
			evas_object_show(img);

			evas_object_size_hint_min_set(img, ELM_SCALE_SIZE(150), ELM_SCALE_SIZE(150));
			elm_object_part_content_set(btn, "icon", img);
		}
	}

	evas_object_show(this->popup);

}

LIBAPI void SRIN::Components::PopupBox::Dismiss() {
	evas_object_del(this->popup);
	this->popup = nullptr;
	this->popupLayout = nullptr;
}

void SRIN::Components::PopupBox::SetOrientation(Elm_Popup_Orient orientation) {
	this->orientation   = orientation;
}

Elm_Popup_Orient SRIN::Components::PopupBox::GetOrientation() {
	return this->orientation;
}
