/*
 * PopupBox.h
 *
 *  Created on: Mar 16, 2016
 *      Contributor:
 *        ib.putu (ib.putu@samsung.com)
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#ifndef POPUPBOX_H_
#define POPUPBOX_H_

#include "TFC/Components/ComponentBase.h"
#include "TFC/Components/BackButtonHandler.h"

namespace TFC {
	namespace Components {
	/**
	 * Component that provides standard popup with a title, messages, and up to three customizable buttons.
	 * It also can be inherited to create custom popup that contains arbitrary content.
	 */
		class LIBAPI PopupBox : public ComponentBase, public BackButtonHandler {
		private:
			std::string  title;
			std::string  message;
			bool shown;

			Evas_Object* popup;
			Evas_Object* popupLayout;
			Evas_Object* root;
			Elm_Popup_Orient orientation;
		protected:
			/**
			 * Method overriden from ComponentBase, creates the UI elements of the component.
			 * Can't be overriden again by its subclasses.
			 *
			 * @param root The root/parent given for this component.
			 *
			 * @return It will return none, since it didn't have any Evas_Object representation.
			 * 		   Note that because of this, any attempt to cast PopupBox to Evas_Object will return nullptr.
			 */
			virtual Evas_Object* CreateComponent(Evas_Object* root) final;

			/**
			 * Method that can be overriden to insert custom content to PopupBox.
			 *
			 * @param root The root/parent given for this component.
			 *
			 * @return Evas_Object that contains the custom content.
			 */
			virtual Evas_Object* CreateContent(Evas_Object* root);

			void SetTitle(const std::string& text);
			std::string& GetTitle();

			void SetOrientation(const Elm_Popup_Orient& orientation);
			Elm_Popup_Orient& GetOrientation();

			void SetMessage(const std::string& text);
			std::string& GetMessageString();

			/**
			 * Method from BackButtonHandler that will be called when the back button is pressed.
			 * The default implementation of PopupBox is to dismiss the popup.
			 *
			 * @return true if the application should be closed after clicking back.
			 * 		   false to cancel closing the application.
			 */
			virtual bool BackButtonClicked();
		public:
			/**
			 * Constructor of the PopupBox component.
			 */
			PopupBox();

			/**
			 * Property that enables getting & setting the title of popup.
			 * The return/parameter type is string reference.
			 */
			Property<PopupBox, std::string&>::GetSet<&PopupBox::GetTitle, &PopupBox::SetTitle> Title;

			/**
			 * Property that enables getting & setting the message text of popup.
			 * The return/parameter type is string reference.
			 */
			Property<PopupBox, std::string&>::GetSet<&PopupBox::GetMessageString, &PopupBox::SetMessage> Message;

			/**
			 * Property that enables getting & setting the orientation of popup.
			 * The return/parameter type is Elm_Popup_Orient reference.
			 */
			Property<PopupBox, Elm_Popup_Orient&>::GetSet<&PopupBox::GetOrientation, &PopupBox::SetOrientation> Orientation;


			/**
			 * Event that will be triggered when the leftmost button is clicked.
			 */
			EFL::EvasSmartEvent eventButtonOneClick;
			/**
			 * Event that will be triggered when the center (or right, if there's no third button) button is clicked.
			 */
			EFL::EvasSmartEvent eventButtonTwoClick;
			/**
			 * Event that will be triggered when the rightmost button is clicked.
			 */
			EFL::EvasSmartEvent eventButtonThreeClick;

			/**
			 * Event that will be triggered when the popup is shown.
			 */
			Event<PopupBox*, void*> eventPopupShown;
			/**
			 * Event that will be triggered when the popup is dismissed.
			 */
			Event<PopupBox*, void*> eventPopupDismissed;

			/**
			 * String that will be used as the leftmost button's text.
			 * If this and buttonOneImage is empty, leftmost button will not be shown.
			 */
			std::string buttonOneText;
			/**
			 * String that will be used as the center (or right, if there's no third button) button's text.
			 * If this and buttonTwoImage is empty, center button will not be shown.
			 */
			std::string buttonTwoText;
			/**
			 * String that will be used as the rightmost button's text.
			 * If this and buttonThreeImage is empty, rightmost button will not be shown.
			 */
			std::string buttonThreeText;

			/**
			 * String that will be used as path of the leftmost button's image.
			 */
			std::string buttonOneImage;
			/**
			 * String that will be used as path of the center (or right, if there's no third button) button's image.
			 */
			std::string buttonTwoImage;
			/**
			 * String that will be used as path of the rightmost button's image.
			 */
			std::string buttonThreeImage;

			/**
			 * Method that initialize the popup's content and show it.
			 * When the popup is being shown, it will also acquire back button handler.
			 */
			
			void Show();

			const bool& IsShown;

			/**
			 * Method that will dismiss the popup, destroy its content and release back button handler.
			 */
			virtual void Dismiss();
		};
	}
}

#endif /* POPUPBOX_H_ */
