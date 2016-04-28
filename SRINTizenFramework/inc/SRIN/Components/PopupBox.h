/*
 * PopupBox.h
 *
 *  Created on: Mar 16, 2016
 *      Author: Ida Bagus Putu Peradnya Dinata
 */

#ifndef POPUPBOX_H_
#define POPUPBOX_H_

#include "SRIN/Components/ComponentBase.h"
#include "SRIN/Components/BackButtonHandler.h"

namespace SRIN {
	namespace Components {
		class LIBAPI PopupBox : public ComponentBase, public BackButtonHandler {
		private:
			std::string  title;
			std::string  message;

			Evas_Object* popup;
			Evas_Object* popupLayout;
			Evas_Object* root;
			Elm_Popup_Orient orientation;
		protected:
			virtual Evas_Object* CreateComponent(Evas_Object* root) final;
			virtual Evas_Object* CreateContent(Evas_Object* root);
			virtual bool BackButtonPressed();

			void SetTitle(const std::string& text);
			std::string& GetTitle();

			void SetOrientation(const Elm_Popup_Orient& orientation);
			Elm_Popup_Orient& GetOrientation();

			void SetMessage(const std::string& text);
			std::string& GetMessageString();
			virtual bool BackButtonClicked();
		public:
			PopupBox();
			Property<PopupBox, std::string&>::GetSet<&PopupBox::GetTitle, &PopupBox::SetTitle> Title;
			Property<PopupBox, std::string&>::GetSet<&PopupBox::GetMessageString, &PopupBox::SetMessage> Message;
			Property<PopupBox, Elm_Popup_Orient&>::GetSet<&PopupBox::GetOrientation, &PopupBox::SetOrientation> Orientation;

			ElementaryEvent buttonOneClick, buttonTwoClick, buttonThreeClick;

			std::string buttonOneText, buttonTwoText, buttonThreeText;
			std::string buttonOneImage, buttonTwoImage, buttonThreeImage;

			void Show();
			virtual void Dismiss();
		};
	}
}

#endif /* POPUPBOX_H_ */
