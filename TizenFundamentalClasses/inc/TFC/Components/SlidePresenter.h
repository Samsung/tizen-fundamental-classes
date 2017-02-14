/*
 *SlidePresenter.h
 *
 *  Created on: Sep 13, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SLIDEPRESENTER_H_
#define SLIDEPRESENTER_H_

#include "TFC/Components/ComponentBase.h"

#include <vector>

namespace TFC {
namespace Components {

class LIBAPI SlidePresenter :
		public ComponentBase,
		public EFL::EFLProxyClass,
		public EventEmitterClass<SlidePresenter>
{
using EventEmitterClass<SlidePresenter>::Event;
public:
	SlidePresenter();
	virtual ~SlidePresenter();

	void InsertPage(Evas_Object* page);
	void InsertPageAt(Evas_Object* page, int index);
	void Remove(int index);

	Event<int> eventPageChanged;
protected:
	virtual Evas_Object* CreateComponent(Evas_Object* root) override;
private:
	std::vector<Evas_Object*> pageList;

	Evas_Object* box;
	Evas_Object* scroller;
	Evas_Object* index;

	EvasObjectEvent eventLayoutResizeInternal;
	EvasSmartEvent eventPageScrolledInternal;
	EvasSmartEvent eventIndexSelectedInternal;

	void OnLayoutResize(EFL::EvasEventSourceInfo objSource, void* event_data);
	void OnPageScrolled(Evas_Object* source, void* event_data);
	void OnIndexSelected(Evas_Object* source, void* event_data);
};


}
}



#endif /* SLIDEPRESENTER_H_ */
