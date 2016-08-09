/*
 * FloatingButton.h
 *
 *  Created on: May 17, 2016
 *      Author: Kevin Winata
 */

#ifndef FLOATINGBUTTON_H_
#define FLOATINGBUTTON_H_

#include "SRIN/Components/ComponentBase.h"

namespace SRIN {
	namespace Components {
		class LIBAPI FloatingButton : public ComponentBase {
		private:
			Evas_Object* naviframe;
			Evas_Object* floatingButton;
			Evas_Object* buttonRight;
			Evas_Object* buttonLeft;
		protected:
			virtual Evas_Object* CreateComponent(Evas_Object* root) final;
		public:
			FloatingButton();
			std::string buttonLeftImage, buttonRightImage;
			EFL::EvasSmartEvent eventButtonLeftClick, eventButtonRightClick;
			bool doubleButton;
			bool movementBlock;
			void SetWhiteBackground();
			void SetButtonStyle(bool left, const std::string& style);
		};
	}
}


#endif /* FLOATINGBUTTON_H_ */
