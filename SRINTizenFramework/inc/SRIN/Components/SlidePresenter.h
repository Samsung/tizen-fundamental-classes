/*
 * SlidePresenter.h
 *
 *  Created on: Sep 13, 2016
 *      Author: Gilang
 */

#ifndef SLIDEPRESENTER_H_
#define SLIDEPRESENTER_H_

#include "SRIN/Components/ComponentBase.h"

#include <vector>

namespace SRIN {
namespace Components {

class LIBAPI SlidePresenter : public ComponentBase
{
private:
	std::vector<Evas_Object*> pageList;

	Evas_Object* box;
	Evas_Object* scroller;
	Evas_Object* index;

	EFL::EvasObjectEvent eventLayoutResize;
	EFL::EvasSmartEvent eventPageScrolled;


	void OnLayoutResize(EFL::EvasObjectEvent* event, EFL::EvasEventSourceInfo* objSource, void* event_data);
	void OnPageScrolled(EFL::EvasSmartEvent* event, Evas_Object* source, void* event_data);
protected:
	virtual Evas_Object* CreateComponent(Evas_Object* root);
public:
	SlidePresenter();
	virtual ~SlidePresenter();

	void InsertPage(Evas_Object* page);
	void InsertPageAt(Evas_Object* page, int index);
	void Remove(int index);
};


}
}



#endif /* SLIDEPRESENTER_H_ */
