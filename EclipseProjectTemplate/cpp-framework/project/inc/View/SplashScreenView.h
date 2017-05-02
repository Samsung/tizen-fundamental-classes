/*
 * SplashScreenView.h
 *
 *  Created on: Apr 11, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SPLASHSCREENVIEW_H_
#define SPLASHSCREENVIEW_H_

#include "$(appName).h"

#include "SRIN/Components/Field.h"

namespace $(appName) {
	namespace View {
		class SplashScreenView: public SRIN::Framework::ViewBase
		{
		private:
			Evas_Object* rootLayout;
			Evas_Object* labelText;
			SRIN::Components::Field fieldText;

		protected:
			virtual Evas_Object* CreateView(Evas_Object* root);

		public:
			SplashScreenView();
			virtual ~SplashScreenView();

			// We can use this pattern to make property of internal component visible to other
			decltype(fieldText.Text)& FieldText;
		};
	}
}



#endif /* SPLASHSCREENVIEW_H_ */
