/*
 * FloatingButton.h
 *
 *  Created on: May 17, 2016
 *      Contributor:
 *        Kevin Winata (k.winata@samsung.com)
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef FLOATINGBUTTON_H_
#define FLOATINGBUTTON_H_

#include "TFC/Components/ComponentBase.h"

namespace TFC {
namespace Components {

/**
* Component that wraps the use of Eext_FloatingButton from Tizen Native EFL Extension.
* It is an interactive, movable, and customizable button that "floats" in the screen.
*/
class LIBAPI FloatingButton :
		public ComponentBase,
		public EFL::EFLProxyClass
{
private:
	Evas_Object* naviframe;
	Evas_Object* floatingButton;
	Evas_Object* buttonLeft;
	Evas_Object* buttonRight;
protected:
	/**
	 * Method overriden from ComponentBase, creates the UI elements of the component.
	 *
	 * @param root The root/parent given for this component.
	 *
	 * @return An Eext_FloatingButton widget.
	 */
	virtual Evas_Object* CreateComponent(Evas_Object* root) final;
public:
	/**
	 * Constructor of FloatingButton component.
	 *
	 * @note This component requires a swallow named "elm.swallow.floatingbutton"
	 * 		 that must be present in the layout of views that use it.
	 */
	FloatingButton();

	/**
	 * String that will be used as path of the leftmost button's image.
	 * If this is empty, leftmost button will not be shown.
	 */
	std::string buttonLeftImage;

	/**
	 * String that will be used as path of the rightmost button's image.
	 * If this is empty or doubleButton is false, rightmost button will not be shown.
	 */
	std::string buttonRightImage;

	/**
	 * Event that will be triggered when the leftmost button is clicked.
	 */
	EvasSmartEvent eventButtonLeftClick;

	/**
	 * Event that will be triggered when the rightmost button is clicked.
	 */
	EvasSmartEvent eventButtonRightClick;

	/**
	 * Boolean that will set the number of buttons on FloatingButton.
	 * If true, then it will enable double button mode. If false, then it will only have one button.
	 */
	bool doubleButton;

	/**
	 * Boolean that will set whether the FloatingButton can be moved or not.
	 * If true, then it will be static. If false, then it will be movable.
	 */
	bool movementBlock;

	/**
	 * Method that can be used to set an alternative, white styled FloatingButton.
	 */
	void SetWhiteBackground();

	/**
	 * Method to set buttons style.
	 *
	 * @param left If true, then the left button will be styled. If false, then the right button.
	 * @param style String to the style resource.
	 */
	void SetButtonStyle(bool left, const std::string& style);
};

}}


#endif /* FLOATINGBUTTON_H_ */
