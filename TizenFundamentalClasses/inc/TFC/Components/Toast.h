/*
 * Toast.h
 *
 *  Created on: Mar 16, 2016
 *      Contributor:
 *        ib.putu (ib.putu@samsung.com)
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#ifndef TOAST_H_
#define TOAST_H_

#include "TFC/Framework/Application.h"
#include "TFC/EFL.h"

/**
 * Component that handles showing toast.
 * It isn't inherited from ComponentBase, instead it provides static methods to show toast.
 */
namespace TFC {
namespace Components {

class LIBAPI Toast : public EFL::EFLProxyClass
{
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
private:
	EvasSmartEvent eventDismiss;
	EvasSmartEvent eventTimeout;

	void OnDismiss(Evas_Object* objSource, void* eventData);
	bool BackButtonPressed(Evas_Object* objSource, void* eventData);
};

}}

#endif /* TOAST_H_ */
