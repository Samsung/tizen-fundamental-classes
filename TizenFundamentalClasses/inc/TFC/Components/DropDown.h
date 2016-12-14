/*
 * DropDown.h
 *
 *  Created on: Mar 21, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 *        Calvin Windoro (calvin.w@samsung.com)
 */

#ifndef TFC_COMP_DROPDOWN_H_
#define TFC_COMP_DROPDOWN_H_



#include "TFC/Components/ComponentBase.h"
#include "TFC/Components/Adapter.h"

namespace TFC {
namespace Components {

/**
 * Component that provides dropdown menu.
 * It uses adapter pattern for appending and removing items,
 * and its class can map texts, contents and data of an item in the dropdown.
 */
class LIBAPI DropDown :
	public ComponentBase,
	EventEmitterClass<DropDown>,
	PropertyClass<DropDown>
{
	using EventEmitterClass<DropDown>::Event;
	using PropertyClass<DropDown>::Property;
public:
	/**
	 * Constructor for DropDown.
	 */
	DropDown();

	/**
	 * Method to programmatically select an item from the dropdown.
	 *
	 * @param item AdapterItem that will be selected.
	 */
	void ItemClick(Adapter::AdapterItem* item);

	/**
	 * Event that will be triggered when selected item in the dropdown changes.
	 */
	Event<void*> ItemSelectionChanged;
protected:
	/**
	 * Method overriden from ComponentBase, creates the UI elements of the component.
	 *
	 * @param root The root/parent given for this component.
	 *
	 * @return The dropdown's button.
	 */
	virtual Evas_Object* CreateComponent(Evas_Object* root) override;
private:
	std::string text;

	EvasSmartEvent eventDropdownButtonClick;
	EvasSmartEvent eventDropdownDismiss;
	EvasObjectEvent eventDropdownBack;

	Adapter* dataSource;
	Evas_Object* parentComponent;
	Evas_Object* dropdownComponent;

	void SetDataSource(Adapter* adapter);
	Adapter* GetDataSource();

	void ShowDropdown();
	void OnDropDownButtonClick(Evas_Object* objSource, void* eventData);

	std::string const& GetText() const { return this->text; }
	void SetText(std::string const& text) { this->text = text; }
public:
	/**
	 * Property that enables getting & setting the adapter of the dropdown.
	 * The return/parameter type is Adapter.
	 */
	Property<Adapter*>::Get<&DropDown::GetDataSource>::Set<&DropDown::SetDataSource> DataSource;

	/**
	 * Property that enables reading of which item is selected in the dropdown.
	 * The return type is void*.
	 */
	/*Property<void*>::Auto::ReadOnly*/void* SelectedItem;

	Property<std::string const&>::Get<&DropDown::GetText>::Set<&DropDown::SetText> Text;

	//TODO : Add empty text setting property
};

/**
 * Simplified AdapterItemClass that's specialized for use in DropDown.
 */
template<typename T>
class DropDownAdapter : public AdapterItemClass<T>
{
public:
	virtual std::string GetString(T* data, Evas_Object *obj, const char *part) final;
	virtual Evas_Object* GetContent(T* data, Evas_Object *obj, const char *part) final;
	/**
	 * Abstract method for providing text in dropdown item.
	 *
	 * @param data Adapter item's data.
	 *
	 * @return The text that will be displayed in item.
	 */
	virtual std::string GetText(T data) = 0;

	/**
	 * Abstract method for providing icon in dropdown item.
	 *
	 * @param data Adapter item's data.
	 *
	 * @return The Evas_Object that will be used as item icon.
	 */
	virtual Evas_Object* GetIcon(T* data, Evas_Object root) = 0;

	/**
	 * Destructor for DropDownAdapter.
	 */
	virtual ~DropDownAdapter() { };
};

}
}

template<typename T>
inline std::string TFC::Components::DropDownAdapter<T>::GetString(T* data, Evas_Object* obj, const char* part)
{
	return GetText(data);
}

template<typename T>
inline Evas_Object* TFC::Components::DropDownAdapter<T>::GetContent(T* data, Evas_Object* obj, const char* part)
{
	return GetIcon(data);
}

#endif /* DROPDOWN_H_ */
