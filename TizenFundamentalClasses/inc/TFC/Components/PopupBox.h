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
class LIBAPI PopupBox :
		public BackButtonHandler,
		public ComponentBase,
		public EFL::EFLProxyClass,
		EventEmitterClass<PopupBox>,
		PropertyClass<PopupBox>
{
	using EventEmitterClass<PopupBox>::Event;
	using PropertyClass<PopupBox>::Property;
public:
	/**
	 * Constructor of the PopupBox component.
	 */
	PopupBox();
	/**
	 * Event that will be triggered when the leftmost button is clicked.
	 */
	EvasSmartEvent eventButtonOneClick;
	/**
	 * Event that will be triggered when the center (or right, if there's no third button) button is clicked.
	 */
	EvasSmartEvent eventButtonTwoClick;
	/**
	 * Event that will be triggered when the rightmost button is clicked.
	 */
	EvasSmartEvent eventButtonThreeClick;

	/**
	 * Event that will be triggered when the popup is shown.
	 */
	Event<void*> eventPopupShown;
	/**
	 * Event that will be triggered when the popup is dismissed.
	 */
	Event<void*> eventPopupDismissed;

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

	/**
	 * Method from BackButtonHandler that will be called when the back button is pressed.
	 * The default implementation of PopupBox is to dismiss the popup.
	 *
	 * @return true if the application should be closed after clicking back.
	 * 		   false to cancel closing the application.
	 */
	virtual bool BackButtonClicked();
private:
	std::string  title;
	std::string  message;

	std::string buttonOneText;
	std::string buttonTwoText;
	std::string buttonThreeText;

	std::string buttonOneImage;
	std::string buttonTwoImage;
	std::string buttonThreeImage;

	Evas_Object* popup;
	Evas_Object* popupLayout;
	Evas_Object* root;
	Elm_Popup_Orient orientation;

	bool shown;

	std::string const& GetButtonOneText() const { return this->buttonOneText; }
	std::string const& GetButtonTwoText() const { return this->buttonTwoText; }
	std::string const& GetButtonThreeText() const { return this->buttonThreeText; }

	std::string const& GetButtonOneImage() const { return this->buttonOneImage; }
	std::string const& GetButtonTwoImage() const { return this->buttonTwoImage; }
	std::string const& GetButtonThreeImage() const { return this->buttonThreeImage; }

	void SetButtonOneText(std::string const& text) { this->buttonOneText = text; }
	void SetButtonTwoText(std::string const& text) { this->buttonTwoText = text; }
	void SetButtonThreeText(std::string const& text) { this->buttonThreeText = text; }

	void SetButtonOneImage(std::string const& image) { this->buttonOneImage = image; }
	void SetButtonTwoImage(std::string const& image) { this->buttonTwoImage = image; }
	void SetButtonThreeImage(std::string const& image) { this->buttonThreeImage = image; }

	void SetTitle(std::string const& text);
	std::string const& GetTitle() const;

	void SetOrientation(Elm_Popup_Orient const& orientation);
	Elm_Popup_Orient const& GetOrientation() const;

	void SetMessage(std::string const& text);
	std::string const& GetMessageString() const;
public:
	/**
	 * Property that enables getting & setting the title of popup.
	 * The return/parameter type is string reference.
	 */
	Property<std::string const&>::Get<&PopupBox::GetTitle>::Set<&PopupBox::SetTitle> Title;

	/**
	 * Property that enables getting & setting the message text of popup.
	 * The return/parameter type is string reference.
	 */
	Property<std::string const&>::Get<&PopupBox::GetMessageString>::Set<&PopupBox::SetMessage> Message;

	/**
	 * Property that enables getting & setting the orientation of popup.
	 * The return/parameter type is Elm_Popup_Orient reference.
	 */
	Property<Elm_Popup_Orient const&>::Get<&PopupBox::GetOrientation>::Set<&PopupBox::SetOrientation> Orientation;

	/**
	 * String that will be used as the leftmost button's text.
	 * If this and buttonOneImage is empty, leftmost button will not be shown.
	 */
	Property<std::string const&>::Get<&PopupBox::GetButtonOneText>::Set<&PopupBox::SetButtonOneText> ButtonOneText;
	/**
	 * String that will be used as the center (or right, if there's no third button) button's text.
	 * If this and buttonTwoImage is empty, center button will not be shown.
	 */
	Property<std::string const&>::Get<&PopupBox::GetButtonTwoText>::Set<&PopupBox::SetButtonTwoText> ButtonTwoText;
	/**
	 * String that will be used as the rightmost button's text.
	 * If this and buttonThreeImage is empty, rightmost button will not be shown.
	 */
	Property<std::string const&>::Get<&PopupBox::GetButtonThreeText>::Set<&PopupBox::SetButtonThreeText> ButtonThreeText;

	/**
	 * String that will be used as path of the leftmost button's image.
	 */
	Property<std::string const&>::Get<&PopupBox::GetButtonOneImage>::Set<&PopupBox::SetButtonOneImage> ButtonOneImage;
	/**
	 * String that will be used as path of the center (or right, if there's no third button) button's image.
	 */
	Property<std::string const&>::Get<&PopupBox::GetButtonTwoImage>::Set<&PopupBox::SetButtonTwoImage> ButtonTwoImage;
	/**
	 * String that will be used as path of the rightmost button's image.
	 */
	Property<std::string const&>::Get<&PopupBox::GetButtonThreeImage>::Set<&PopupBox::SetButtonOneImage> ButtonThreeImage;
};

}}

#endif /* POPUPBOX_H_ */
