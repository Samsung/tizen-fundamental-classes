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

/**
 * Component that handles showing toast.
 * It isn't inherited from ComponentBase, instead it provides static methods to show toast.
 */
namespace SRIN {
	namespace Components {
		class LIBAPI Toast : public EventClass {
		private:
			void OnDismiss(EFL::EvasSmartEvent* event, Evas_Object* objSource, void* eventData);
			bool BackButtonPressed(EFL::EvasSmartEvent* event, Evas_Object* objSource, void* eventData);
			EFL::EvasSmartEvent eventDismiss;
		public:
			/**
			 * Constructor for Toast class.
			 */
			Toast();

			/**
			 * Method to show a toast with default timeout.
			 *
			 * @param message String that will be shown inside the toast.
			 */
			static void Show(const std::string& message);

			/**
			 * Method to show a toast with custom timeout.
			 *
			 * @param message String that will be shown inside the toast.
			 * @param timeout How long the toast will be shown.
			 */
			static void Show(const std::string& message, double timeout);
		};
	}
}

#endif /* TOAST_H_ */
