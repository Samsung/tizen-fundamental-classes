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
		class LIBAPI Toast : public EventClass {
		private:
			Evas_Object* popup;
			void OnDismiss(ElementaryEvent* event, Evas_Object* objSource, void* eventData);
			bool BackButtonPressed(ElementaryEvent* event, Evas_Object* objSource, void* eventData);
			ElementaryEvent onDismiss;
		public:
			Toast();

			static void Show(const std::string& message);
			static void Show(const std::string& message, double timeout);
		};
	}
}

#endif /* TOAST_H_ */
