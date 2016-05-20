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
			Evas_Object* buttonLeft;
			Evas_Object* buttonRight;
		protected:
			virtual Evas_Object* CreateComponent(Evas_Object* root) final;
		public:
			FloatingButton();
			std::string buttonLeftImage, buttonRightImage;
			ElementaryEvent eventButtonLeftClick, eventButtonRightClick;
			bool doubleButton;
		};
	}
}


#endif /* FLOATINGBUTTON_H_ */
