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
 *    Components/Toast.h
 *
 * Wrapper component to display toast message in UI
 *
 * Created on:  Mar 16, 2016
 * Author:      Ida Bagus Putu Peradnya Dinata (ib.putu@samsung.com)
 * Contributor: Gilang Mentari Hamidy (g.hamidy@samsung.com)
 *              Kevin Winata (k.winata@samsung.com)
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
