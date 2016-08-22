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
/**
 * Component that provides pagination UI.
 * It consists of a page indicator and two buttons, next and previous, to navigate through pages.
 * It also provides an event that will indicate navigation, and
 * can be used to attach callback that implements the actual content changes.
 */
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
	/**
	 * Method overriden from ComponentBase, creates the UI elements of the component.
	 *
	 * @param root The root/parent given for this component.
	 *
	 * @return A box (Elm_Box) that contains both the content and the pagination UI.
	 */
	virtual Evas_Object* CreateComponent(Evas_Object* root);

public:
	/**
	 * Constructor for Pager component.
	 */
	Pager();

	/**
	 * Event that will be triggered when next or previous button is clicked.
	 * It will contain an integer that indicates which page that has to be displayed.
	 */
	Event<Pager*, int> eventNavigate;

	/**
	 * Event that will be triggered when user click previous button while on the first page.
	 * This will only happen when UnderflowEnable is true.
	 */
	Event<Pager*> eventUnderflow;

	/**
	 * Event that will be triggered when user click next button while on the last page.
	 * This will only happen when OverflowEnable is true.
	 */
	Event<Pager*> eventOverflow;

	/**
	 * Contents that want to be paginated.
	 *
	 * @note The contents will not change itself when user navigates the pages.
	 * 		 You will have to implement how the content between pages change on Navigate event
	 * 		 by changing what's inside the Evas_Object that PagerContent points to.
	 */
	Property<Pager, Evas_Object*>::Auto::ReadWrite PagerContent;

	/**
	 * Current page.
	 */
	Property<Pager, int>::Auto::ReadOnly CurrentIndex;

	/**
	 * Number of pages.
	 */
	Property<Pager, int>::Auto::ReadWrite MaxIndex;

	/**
	 * True if you want to display some content before the first page.
	 * False if you don't want to display anything and simply disables previous navigation in the first page.
	 */
	Property<Pager, bool>::Auto::ReadWrite UnderflowEnable;

	/**
	 * True if you want to display some content after the last page.
	 * False if you don't want to display anything and simply disables next navigation in the last page.
	 */
	Property<Pager, bool>::Auto::ReadWrite OverflowEnable;

	/**
	 * Text that will be displayed in-between buttons before the first page.
	 * Replaces page indicator.
	 */
	Property<Pager, std::string>::Auto::ReadWrite UnderflowString;

	/**
	 * Text that will be displayed in-between buttons after the last page.
	 * Replaces page indicator.
	 */
	Property<Pager, std::string>::Auto::ReadWrite OverflowString;
};
}
}



#endif /* PAGER_H_ */
