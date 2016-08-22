/*
 * Adapter.h
 *
 *  Created on: Mar 21, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_COMP_ADAPTER_H_
#define SRIN_COMP_ADAPTER_H_

#include "SRIN/Core.h"
#include "SRIN/EFL.h"

#include <list>
#include <vector>
#include <unordered_map>

namespace SRIN {
namespace Components {

/**
 * Base class for adapter item.
 */
class LIBAPI AdapterItemClassBase
{
public:
	/**
	 * Destructor for AdapterItemClassBase.
	 */
	virtual ~AdapterItemClassBase();

	/**
	 * Abstract method for providing string representation from a part of an adapter item.
	 *
	 * @param data Adapter item's data.
	 * @param obj Evas_Object that represents the item.
	 * @param part Corresponding part's name.
	 *
	 * @return The string representation.
	 */
	virtual std::string GetString(void* data, Evas_Object* obj, const char* part) = 0;

	/**
	 * Abstract method for providing Evas_Object representation from a part of an adapter item.
	 *
	 * @param data Adapter item's data.
	 * @param obj Evas_Object that represents the item.
	 * @param part Corresponding part's name.
	 *
	 * @return The Evas_Object representation.
	 */
	virtual Evas_Object* GetContent(void* data, Evas_Object* obj, const char* part) = 0;

	/**
	 * Abstract method to deallocate item's data.
	 *
	 * @param data Adapter item's data.
	 */
	virtual void Deallocator(void* data) = 0;
};

/**
 * Template base class for adapter item.
 */
template<class T>
class AdapterItemClass : public virtual AdapterItemClassBase
{
protected:
	/**
	 * Constructor for AdapterItemClass.
	 */
	AdapterItemClass();
public:
	/**
	 * Abstract method for providing string representation from a part of an adapter item.
	 *
	 * @param data Adapter item's data.
	 * @param obj Evas_Object that represents the item.
	 * @param part Corresponding part's name.
	 *
	 * @return The string representation.
	 */
	virtual std::string GetString(void* data, Evas_Object *obj, const char *part) final;

	/**
	 * Abstract method for providing Evas_Object representation from a part of an adapter item.
	 *
	 * @param data Adapter item's data.
	 * @param obj Evas_Object that represents the item.
	 * @param part Corresponding part's name.
	 *
	 * @return The Evas_Object representation.
	 */
	virtual Evas_Object* GetContent(void* data, Evas_Object *obj, const char *part) final;

	/**
	 * Abstract method to deallocate item's data.
	 *
	 * @param data Adapter item's data.
	 */
	virtual void Deallocator(void* data);

	/**
	 * Abstract method that implement AdapterItemClassBase's GetString so it use template argument type instead.
	 *
	 * @param data Adapter item's data.
	 * @param obj Evas_Object that represents the item.
	 * @param part Corresponding part's name.
	 *
	 * @return The string representation.
	 */
	virtual std::string GetString(T* data, Evas_Object *obj, const char *part) = 0;

	/**
	 * Abstract method that implement AdapterItemClassBase's GetContent so it use template argument type instead.
	 *
	 * @param data Adapter item's data.
	 * @param obj Evas_Object that represents the item.
	 * @param part Corresponding part's name.
	 *
	 * @return The Evas_Object representation.
	 */
	virtual Evas_Object* GetContent(T* data, Evas_Object *obj, const char *part) = 0;

	/**
	 * Destructor for AdapterItemClass.
	 */
	virtual ~AdapterItemClass() { };
};

/**
 * Class that contains a list of adapter items.
 * Provides basic list primitives such as adding items, removing items, and clearing the list.
 */
class LIBAPI Adapter
{
public:
	/**
	 * Structure of item in the adapter's list.
	 * It can contain arbitrary data, its class information, and pointer to Elm_Object_Item.
	 */
	struct AdapterItem
	{
		void* data;
		AdapterItemClassBase* itemClass;
		Elm_Object_Item* objectItem;
	};

	/**
	 * Constructor of Adapter.
	 */
	Adapter();

	/**
	 * Forbid copy constructor of Adapter.
	 */
	Adapter(Adapter&) = delete;

	/**
	 * Destructor of Adapter.
	 */
	~Adapter();

	/**
	 * Method to add item to adapter's list.
	 *
	 * @param data Item's data.
	 * @param itemClass Class of the item.
	 */
	template<class T>
	void AddItem(T* data, AdapterItemClass<T>* itemClass);

	/**
	 * Method to remove item from adapter's list.
	 *
	 * @param data Data of items that you want to delete.
	 */
	template<class T>
	void RemoveItem(T* data);

	/**
	 * Method to clear all items in the adapter's list.
	 *
	 * @param deallocate If true, then it will call Deallocator for every item in the list as well.
	 * 					 If false, then it will only remove it from list.
	 */
	void Clear(bool deallocate = false);

	/**
	 * Method to get the all items in adapter's list.
	 *
	 * @return Reference to vector that contains the items.
	 */
	std::vector<AdapterItem>& GetAll();

	/**
	 * Method to get number of items in adapter's list.
	 *
	 * @return Number of items.
	 */
	int GetCount();


	/**
	 * Event that will be triggered when an item is added to the list.
	 */
	Event<Adapter*, AdapterItem*> eventItemAdd;

	/**
	 * Event that will be triggered when items are removed from the list.
	 */
	Event<Adapter*, AdapterItem*> eventItemRemove;

protected:
	void AddItemInternal(void* data, AdapterItemClassBase* itemClass);
	void RemoveItemInternal(void* data);
private:
	std::vector<AdapterItem> adapterItems;


};

/**
 * Subclass of Adapter that exclusively contains BasicAdapterItems, which are simple item containing string value.
 */
class LIBAPI BasicListAdapter : public Adapter
{
public:
	/**
	 * Simple structure that contains index and value/data.
	 */
	struct BasicAdapterItem
	{
		int index;
		std::string value;
	};

	/**
	 * Constructor for BasicListAdapter.
	 */
	BasicListAdapter();
	/**
	 * Constructor for BasicListAdapter that can construct some initial items.
	 */
	BasicListAdapter(std::initializer_list<std::string> init);

	/**
	 * Destructor for BasicListAdapter.
	 */
	~BasicListAdapter();

	/**
	 * Method to add item to adapter's list.
	 *
	 * @param string Item's data.
	 */
	void AddItem(std::string string);

	/**
	 * Method to remove item from adapter's list.
	 *
	 * @param string Data of items that you want to delete.
	 */
	void RemoveItem(std::string string);
private:
	class BasicListItemClass : public AdapterItemClass<BasicAdapterItem>
	{
	public:
		virtual std::string GetString(BasicAdapterItem* data, Evas_Object *obj, const char *part);
		virtual Evas_Object* GetContent(BasicAdapterItem* data, Evas_Object *obj, const char *part);
	} static basicListItemClass;
};

}
}

template<class T>
inline SRIN::Components::AdapterItemClass<T>::AdapterItemClass()
{
}

template<class T>
inline std::string SRIN::Components::AdapterItemClass<T>::GetString(void* data, Evas_Object* obj, const char* part)
{
	return this->GetString(reinterpret_cast<T*>(data), obj, part);
}

template<class T>
inline Evas_Object* SRIN::Components::AdapterItemClass<T>::GetContent(void* data, Evas_Object* obj, const char* part)
{
	return this->GetContent(reinterpret_cast<T*>(data), obj, part);
}

template<class T>
inline void SRIN::Components::AdapterItemClass<T>::Deallocator(void* data)
{
	auto obj = reinterpret_cast<T*>(data);
	delete obj;
}

template<class T>
inline void SRIN::Components::Adapter::AddItem(T* data, AdapterItemClass<T>* itemClass)
{
	AddItemInternal(data, itemClass);
}

template<class T>
inline void SRIN::Components::Adapter::RemoveItem(T* data)
{
	RemoveItemInternal(data);
}

#endif /* ADAPTER_H_ */
