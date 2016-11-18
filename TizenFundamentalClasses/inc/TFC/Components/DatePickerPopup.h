/*
 * DatePickerPopup.h
 *
 *  Created on: Mar 16, 2016
 *      Contributor:
 *        ib.putu (ib.putu@samsung.com)
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef DATEPICKERPOPUP_H_
#define DATEPICKERPOPUP_H_

#include "TFC/Framework/Application.h"
#include "TFC/Components/PopupBox.h"
#include "TFC/Components/ComponentBase.h"
#include "TFC/Components/BackButtonHandler.h"
#include <ctime>

namespace TFC {
namespace Components {

/**
 * Inner component of DatePickerPopup to create popup when date picker button was selected.
 * Don't using this class in your tizen project, instead please use DatePickerPopup.
 */
class LIBAPI DatePickerPopupMenu : public PopupBox {
private:
	bool 			classicTheme;
	Evas_Object* 	dateTime;
	std::tm      	selectedDate;
	std::string		strDate;
protected:
	/**
	 * Method that can be overriden to create custom date picker PopupBox.
	 *
	 * @param root The root/parent given for this component.
	 *
	 * @return Evas_Object that contains the custom content.
	 */
	virtual Evas_Object* CreateContent(Evas_Object* root);
public:
	/**
	 * Constructor of the DatePickerPopupMenu class.
	 */
	DatePickerPopupMenu();

	/**
	 * Method that return selected date.
	 *
	 * @return std::tm& tm struct of selected date/time.
	 */
	std::tm const& GetSelectedDate();

	/**
	 * Method that return string of selected date.
	 *
	 * @return std::string& string of selected date/time.
	 */
	std::string const& GetFormatedSelectedDate();

	/**
	 * Method that change UI of Date Picker.
	 *
	 * @param classic true if use classic (DatePicker) UI, or false to use modern (Calendar) UI.
	 */
	void UseClassicTheme(bool const& classic);

	/**
	 * Method that change init date value of datepicker
	 *
	 * @param date tm struct of selected date/time.
	 */
	void SetSelectedDate(std::tm const& date);

	/**
	 * Method that change init date value of datepicker.
	 * format: YYYY-MM-DD
	 *
	 * @param date The string of date.
	 */
	void SetFormatedSelectedDate(std::string const& date);

	/**
	 * Event that will triggered when date changed.
	 *
	 */
	EvasSmartEvent eventDateChanged;
};

/**
 * Component that provides date picker popup.
 * This component provide "classic" date picker popup using date picker UI, or
 * modern date picker using calendar UI.
 */
class LIBAPI DatePickerPopup :
		public ComponentBase,
		public EFL::EFLProxyClass,
		PropertyClass<DatePickerPopup>
{
	using PropertyClass<DatePickerPopup>::Property;
private:
	Evas_Object* 	buttonRoot;

	DatePickerPopupMenu datePopupMenu;
	std::tm      		selectedDate;

	std::string 	 buttonText, formatedDate;

	EvasSmartEvent  eventButtonClick;

	void DatePickerButtonClick(Evas_Object* objSource, void* eventData);
	void DatePickerPopupOkButtonClick(Evas_Object* objSource, void* eventData);
	void DatePickerPopupCancelButtonClick(Evas_Object* objSource, void* eventData);
	void OnDateChangedCb(Evas_Object* objSource, void* eventData);
protected:
	/**
	 * Method that can be overriden to create custom date picker PopupBox.
	 *
	 * @param root The root/parent given for this component.
	 *
	 * @return Evas_Object that contains the custom content.
	 */
	virtual Evas_Object* CreateComponent(Evas_Object* root) override;

	void SetTitle(std::string const& text);
	std::string const& GetTitle() const;

	void SetHint(std::string const& text);
	std::string const& GetHint() const;

	void SetOrientation(Elm_Popup_Orient const& orientation);
	Elm_Popup_Orient const& GetOrientation() const;
public:
	/**
	 * Constructor of the DatePickerPopup Component.
	 */
	DatePickerPopup();

	/**
	 * Property that enables getting & setting popup title.
	 * The return/parameter type is string.
	 */
	Property<std::string const&>::GetSet<&DatePickerPopup::GetTitle, &DatePickerPopup::SetTitle> Title;

	/**
	 * Property that enables getting & setting popup hint.
	 * The return/parameter type is string.
	 */
	Property<std::string const&>::GetSet<&DatePickerPopup::GetHint, &DatePickerPopup::SetHint> Hint;

	/**
	 * Property that enables getting & setting popup orientation.
	 * The return/parameter type is Elm_Popup_Orient.
	 * @see {Elm_Popup_Orient}
	 */
	Property<Elm_Popup_Orient const&>::GetSet<&DatePickerPopup::GetOrientation, &DatePickerPopup::SetOrientation> Orientation;

	/**
	 * Method that return selected date.
	 *
	 * @return std::tm& tm struct of selected date/time.
	 */
	std::tm const& GetSelectedDate() const;

	/**
	 * Method that return string of selected date.
	 *
	 * @return std::string& string of selected date/time.
	 */
	std::string const& GetFormatedSelectedDate() const;

	/**
	 * Method that change UI of Date Picker.
	 *
	 * @param classic true if use classic (DatePicker) UI, or false to use modern (Calendar) UI.
	 */
	void UseClassicTheme(bool const& classic);

	/**
	 * Method that change init date value of datepicker
	 *
	 * @param date tm struct of selected date/time.
	 */
	void SetSelectedDate(std::tm const& date);

	/**
	 * Method that change init date value of datepicker.
	 * format: YYYY-MM-DD
	 *
	 * @param date The string of date.
	 */
	void SetFormatedSelectedDate(std::string const& date);
};

}}

#endif /* DATEPICKERPOPUP_H_ */
