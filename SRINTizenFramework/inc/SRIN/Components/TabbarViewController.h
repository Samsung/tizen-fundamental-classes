/*
 * TabbarControllerBase.h
 *
 *  Created on: Feb 19, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRINFW_TABBARVIEWCONTROLLER_H_
#define SRINFW_TABBARVIEWCONTROLLER_H_

#include "SRIN/Framework/Application.h"

namespace SRIN {
namespace Components {
class LIBAPI TabbarViewController:
		public Framework::ControllerBase,
		public Framework::ViewBase
{
protected:
	virtual Evas_Object* CreateView(Evas_Object* root) final;
public:
	TabbarViewController(SRIN::Framework::ControllerManager* m, CString controllerName);
};
}
}

#endif /* SRINFW_TABBARVIEWCONTROLLER_H_ */
