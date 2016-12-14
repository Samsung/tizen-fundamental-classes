/*
 * PopupBox.cpp
 *
 *  Created on: Mar 16, 2016
 *      Contributor:
 *        ib.putu (ib.putu@samsung.com)
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Components/PopupBox.h"
#include <string>

using namespace TFC::Components;

LIBAPI Evas_Object* TFC::Components::PopupBox::CreateComponent(Evas_Object* root) {
	this->root = root;
	return nullptr;
}

LIBAPI Evas_Object* TFC::Components::PopupBox::CreateContent(Evas_Object* root)
{
	return nullptr;
}

LIBAPI void TFC::Components::PopupBox::SetTitle(std::string const& text)
{
	this->title = text;
	elm_object_part_text_set(this->popup, "title,text", this->title.c_str());
}

LIBAPI std::string const& TFC::Components::PopupBox::GetTitle() const
{
	return this->title;
}

LIBAPI void TFC::Components::PopupBox::SetMessage(std::string const& text)
{
	this->message = text;
}

LIBAPI std::string const& TFC::Components::PopupBox::GetMessageString() const
{
	return this->message;
}

LIBAPI TFC::Components::PopupBox::PopupBox() :
		IsShown(shown),
		popup(nullptr),
		popupLayout(nullptr),
		root(nullptr),
		orientation(Elm_Popup_Orient::ELM_POPUP_ORIENT_BOTTOM),
		shown(false),
		Title(this),
		Message(this),
		Orientation(this),
		ButtonOneText(this),
		ButtonTwoText(this),
		ButtonThreeText(this),
		ButtonOneImage(this),
		ButtonTwoImage(this),
		ButtonThreeImage(this)
{
	Visible = false;
	title = "";
	message = "";

	buttonOneText = "";
	buttonTwoText = "";
	buttonThreeText = "";

	buttonOneImage = "";
	buttonTwoImage = "";
	buttonThreeImage = "";
}

LIBAPI void TFC::Components::PopupBox::Show() {
	this->popup = elm_popup_add(root);
	BackButtonHandler::Acquire();

	elm_popup_orient_set(this->popup, this->orientation);
	this->popupLayout = CreateContent(this->root);

	if(title.size() > 0)
		elm_object_part_text_set(this->popup, "title,text", this->title.c_str());

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
		//evas_object_smart_callback_add(btn, "clicked", EFL::EvasSmartEventHandler, &eventButtonOneClick);
		eventButtonOneClick.Bind(btn, "clicked");

		if(buttonOneImage.size() > 0)
		{
			Evas_Object* img = elm_icon_add(btn);
			std::string path = TFC::Framework::ApplicationBase::GetResourcePath(buttonOneImage.c_str());
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
		//evas_object_smart_callback_add(btn, "clicked", EFL::EvasSmartEventHandler, &eventButtonTwoClick);
		eventButtonTwoClick.Bind(btn, "clicked");

		if(buttonTwoImage.size() > 0)
		{
			Evas_Object* img = elm_icon_add(btn);
			std::string path = TFC::Framework::ApplicationBase::GetResourcePath(buttonTwoImage.c_str());
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
		//evas_object_smart_callback_add(btn, "clicked", EFL::EvasSmartEventHandler, &eventButtonThreeClick);
		eventButtonThreeClick.Bind(btn, "clicked");

		if(buttonThreeImage.size() > 0)
		{
			Evas_Object* img = elm_icon_add(btn);
			std::string path = TFC::Framework::ApplicationBase::GetResourcePath(buttonThreeImage.c_str());
			elm_image_file_set(img, path.c_str(), nullptr);
			evas_object_show(img);

			evas_object_size_hint_min_set(img, ELM_SCALE_SIZE(150), ELM_SCALE_SIZE(150));
			elm_object_part_content_set(btn, "icon", img);
		}
	}

	evas_object_show(this->popup);
	shown = true;
	eventPopupShown(this, nullptr);

}

LIBAPI void TFC::Components::PopupBox::Dismiss() {
	evas_object_del(this->popup);
	this->popup = nullptr;
	this->popupLayout = nullptr;
	BackButtonHandler::Release();
	shown = false;
	eventPopupDismissed(this, nullptr);

}

void TFC::Components::PopupBox::SetOrientation(Elm_Popup_Orient const& orientation) {
	this->orientation   = orientation;
}

Elm_Popup_Orient const& TFC::Components::PopupBox::GetOrientation() const{
	return this->orientation;
}

LIBAPI bool TFC::Components::PopupBox::BackButtonClicked()
{
	//dlog_print(DLOG_DEBUG, LOG_TAG, "Dismiss via back button");
	Dismiss();
	return false;
}
