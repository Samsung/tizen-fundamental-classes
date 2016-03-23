/*
 * DropDown.h
 *
 *  Created on: Mar 21, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_COMP_DROPDOWN_H_
#define SRIN_COMP_DROPDOWN_H_



#include "SRIN/Components/ComponentBase.h"
#include "SRIN/Components/Adapter.h"

namespace SRIN {
namespace Components {

class LIBAPI DropDown : public ComponentBase
{
private:
	Evas_Object* parentComponent;
	Evas_Object* dropdownComponent;
	Adapter* dataSource;

	ElementaryEvent dropdownButtonClick;
	ElementaryEvent dropdownDismiss;
	std::string text;

	void SetDataSource(Adapter* adapter);
	Adapter* GetDataSource();

	void ShowDropdown();
	void OnDropDownButtonClick(ElementaryEvent* viewSource, Evas_Object* objSource, void* eventData);
protected:
	virtual Evas_Object* CreateComponent(Evas_Object* root);

public:
	DropDown();

	void ItemClick(Adapter::AdapterItem* item);

	Event<DropDown*, void*> ItemSelectionChanged;

	Property<DropDown, Adapter*>::GetSet<&DropDown::GetDataSource, &DropDown::SetDataSource> DataSource;
	Property<DropDown, void*>::Auto::ReadOnly SelectedItem;
};

template<typename T>
class DropDownAdapter : public AdapterItemClass<T>
{
public:
	virtual std::string GetString(T* data, Evas_Object *obj, const char *part) final;
	virtual Evas_Object* GetContent(T* data, Evas_Object *obj, const char *part) final;
	virtual std::string GetText(T data) = 0;
	virtual Evas_Object* GetIcon(T* data, Evas_Object root) = 0;
	virtual ~DropDownAdapter() { };
};

}
}

template<typename T>
inline std::string SRIN::Components::DropDownAdapter<T>::GetString(T* data, Evas_Object* obj, const char* part)
{
	return GetText(data);
}

template<typename T>
inline Evas_Object* SRIN::Components::DropDownAdapter<T>::GetContent(T* data, Evas_Object* obj, const char* part)
{
	return GetIcon(data);
}

#endif /* DROPDOWN_H_ */
