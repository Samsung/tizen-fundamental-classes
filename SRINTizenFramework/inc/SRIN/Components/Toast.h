/*
 * Toast.h
 *
 *  Created on: Mar 16, 2016
 *      Author: Ida Bagus Putu Peradnya Dinata
 */

#ifndef TOAST_H_
#define TOAST_H_

#include "SRIN/Framework/Application.h"
#include "SRIN/Components/ComponentBase.h"

namespace SRIN {
	namespace Components {
		class LIBAPI Toast : public ComponentBase {
		private:
			Evas_Object* popup;
			void OnDismissEnded(Framework::ViewBase* viewSource, Evas_Object* objSource, void* eventData);

			ElementaryEvent onBackButtonPressed;
		protected:
			std::string message;
			virtual Evas_Object* CreateComponent(Evas_Object* root) final;
			virtual Evas_Object* CreateContent(Evas_Object* root);
			virtual bool BackButtonPressed();
		public:
			Toast();

			void SetMessage(const std::string& text);
			std::string& GetMessageString();

			void Show();
			void Show(double timeout);
			void Dismiss();
		};
	}
}

#endif /* TOAST_H_ */
