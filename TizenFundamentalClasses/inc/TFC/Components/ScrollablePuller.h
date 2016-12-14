/*
 * ScrollablePuller.h
 *
 *  Created on: Nov 18, 2016
 *      Author: Kevin Winata
 */

#ifndef TFC_COMPONENTS_SCROLLABLEPULLER_H_
#define TFC_COMPONENTS_SCROLLABLEPULLER_H_

#include "TFC/Components/ComponentBase.h"

namespace TFC {
namespace Components {

class LIBAPI ScrollablePuller :
		public ComponentBase,
		public EFL::EFLProxyClass,
		EventEmitterClass<ScrollablePuller>,
		PropertyClass<ScrollablePuller>
{
	using PropertyClass<ScrollablePuller>::Property;
	using EventEmitterClass<ScrollablePuller>::Event;

private :
	Evas_Object* rootLayout;
	Evas_Object* puller;
	Evas_Object* box;
	Evas_Object* bgTop;
	Evas_Object* bgBottom;

	Evas_Coord width;
	Evas_Coord height;
	Evas_Coord posX;
	Evas_Coord posY;
	bool pullerShown;

	EvasObjectEvent eventResize;
	EvasSmartEvent eventScrollTop;
	EvasSmartEvent eventScrollDown;
	EvasSmartEvent eventPulledEnd;
	EvasSmartEvent eventPullerScroll;
	EvasSmartEvent eventPullerStop;

	Evas_Object* scrollable;
	Evas_Object* GetScrollable() { return scrollable; }
	void SetScrollable(Evas_Object* scroll) { scrollable = scroll; }

	std::string partName;
	std::string const& GetPartName() const { return partName; }
	void SetPartName(std::string const& name) { partName = name; }

	std::size_t pullerSize;
	std::size_t GetPullerSize() const { return pullerSize; }
	void SetPullerSize(std::size_t const& size) { pullerSize = size; }

	void OnResize(EFL::EvasEventSourceInfo objSource, void* event_data);
	void OnScrollTop(Evas_Object* obj, void* eventData);
	void OnScrollDown(Evas_Object* obj, void* eventData);
	void OnPulledEnd(Evas_Object* obj, void* eventData);
	void OnPullerScroll(Evas_Object* obj, void* eventData);
	void OnPullerStop(Evas_Object* obj, void* eventData);
protected :
	virtual Evas_Object* CreateComponent(Evas_Object* root) final;
public :
	ScrollablePuller();
	~ScrollablePuller();

	Event<void*> eventPull;
	Event<Evas_Object**> eventPullerContentFill;

	Property<Evas_Object*>::Get<&ScrollablePuller::GetScrollable>::Set<&ScrollablePuller::SetScrollable> Scrollable;
	Property<std::string const&>::Get<&ScrollablePuller::GetPartName>::Set<&ScrollablePuller::SetPartName> PartName;
	Property<std::size_t>::Get<&ScrollablePuller::GetPullerSize>::Set<&ScrollablePuller::SetPullerSize> PullerSize;

	void ResetPuller();
	void TogglePuller(bool enable);
};

}
}



#endif /* TFC_COMPONENTS_SCROLLABLEPULLER_H_ */
