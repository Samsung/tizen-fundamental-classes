/*
 * Pager.h
 *
 *  Created on: Mar 29, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_COMP_PAGER_H_
#define SRIN_COMP_PAGER_H_

#include "SRIN/Components/ComponentBase.h"

namespace SRIN {
namespace Components {
class LIBAPI Pager: public ComponentBase
{
private:
	std::string buttonPrevStyle;
	std::string buttonNextStyle;

	Evas_Object* pagerBox;
	Evas_Object* pagerTop;
	Evas_Object* pagerBottom;

	EFL::EdjeSignalEvent eventClickSignal;

	void OnClickSignal(EFL::EdjeSignalEvent* event, Evas_Object* source, EFL::EdjeSignalInfo signalInfo);
	void NextPage();
	void PrevPage();
protected:
	virtual Evas_Object* CreateComponent(Evas_Object* root);

public:
	Pager();


	Event<Pager*, int> eventNavigate;
	Event<Pager*> eventUnderflow;
	Event<Pager*> eventOverflow;

	Property<Pager, Evas_Object*>::Auto::ReadWrite PagerContent;
	Property<Pager, int>::Auto::ReadOnly CurrentIndex;
	Property<Pager, int>::Auto::ReadWrite MaxIndex;
	Property<Pager, bool>::Auto::ReadWrite UnderflowEnable;
	Property<Pager, bool>::Auto::ReadWrite OverflowEnable;
	Property<Pager, std::string>::Auto::ReadWrite UnderflowString;
	Property<Pager, std::string>::Auto::ReadWrite OverflowString;
};
}
}



#endif /* PAGER_H_ */
