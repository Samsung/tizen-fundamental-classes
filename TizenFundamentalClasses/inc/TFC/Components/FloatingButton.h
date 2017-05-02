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
 *    Components/FloatingButton.h
 *
 * Components which displays floating button in layout
 *
 * Created on:  May 17, 2016
 * Author:		 Kevin Winata (k.winata@samsung.com)
 * Contributor: Gilang Mentari Hamidy (g.hamidy@samsung.com)
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
		public EFL::EFLProxyClass,
		PropertyClass<FloatingButton>
{
	using PropertyClass<FloatingButton>::Property;

public:
	/**
	 * Constructor of FloatingButton component.
	 *
	 * @note This component requires a swallow named "elm.swallow.floatingbutton"
	 * 		 that must be present in the layout of views that use it.
	 */
	FloatingButton();
	/**
	 * Event that will be triggered when the leftmost button is clicked.
	 */
	EvasSmartEvent eventButtonLeftClick;

	/**
	 * Event that will be triggered when the rightmost button is clicked.
	 */
	EvasSmartEvent eventButtonRightClick;

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
private:
	std::string buttonLeftImage;
	std::string buttonRightImage;

	Evas_Object* naviframe;
	Evas_Object* floatingButton;
	Evas_Object* buttonLeft;
	Evas_Object* buttonRight;

	bool doubleButton;
	bool movementBlock;

	std::string const& GetButtonLeftImage() const { return this->buttonLeftImage; }
	std::string const& GetButtonRightImage() const { return this->buttonRightImage; }
	bool GetDoubleButton() const { return this->doubleButton; }
	bool GetMovementBlock() const { return this->movementBlock; }

	void SetButtonLeftImage(std::string const& image) { this->buttonLeftImage = image; }
	void SetButtonRightImage(std::string const& image) { this->buttonRightImage = image; }
	void SetDoubleButton(bool const& doubleButton) { this->doubleButton = doubleButton; }
	void SetMovementBlock(bool const& movementBlock) { this->movementBlock = movementBlock; }
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
	 * String that will be used as path of the leftmost button's image.
	 * If this is empty, leftmost button will not be shown.
	 */
	Property<std::string const&>::Get<&FloatingButton::GetButtonLeftImage>::Set<&FloatingButton::SetButtonLeftImage> ButtonLeftImage;

	/**
	 * String that will be used as path of the rightmost button's image.
	 * If this is empty or doubleButton is false, rightmost button will not be shown.
	 */
	Property<std::string const&>::Get<&FloatingButton::GetButtonRightImage>::Set<&FloatingButton::SetButtonRightImage> ButtonRightImage;
	/**
	 * Boolean that will set the number of buttons on FloatingButton.
	 * If true, then it will enable double button mode. If false, then it will only have one button.
	 */
	Property<bool>::Get<&FloatingButton::GetDoubleButton>::Set<&FloatingButton::SetDoubleButton> DoubleButton;

	/**
	 * Boolean that will set whether the FloatingButton can be moved or not.
	 * If true, then it will be static. If false, then it will be movable.
	 */
	Property<bool>::Get<&FloatingButton::GetMovementBlock>::Set<&FloatingButton::SetMovementBlock> MovementBlock;
};

}}


#endif /* FLOATINGBUTTON_H_ */
