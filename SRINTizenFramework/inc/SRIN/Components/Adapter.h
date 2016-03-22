/*
 * Adapter.h
 *
 *  Created on: Mar 21, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_COMP_ADAPTER_H_
#define SRIN_COMP_ADAPTER_H_

#include "SRIN/Core.h"
#include "SRIN/ELMInterface.h"

#include <list>
#include <unordered_map>

namespace SRIN {
namespace Components {

class LIBAPI AdapterItemClassBase
{
public:
	virtual ~AdapterItemClassBase();

	virtual std::string GetString(void* data, Evas_Object* obj, const char* part) = 0;
	virtual Evas_Object* GetContent(void* data, Evas_Object* obj, const char* part) = 0;
	virtual void Deallocator(void* data) = 0;
};

template<class T>
class AdapterItemClass : public virtual AdapterItemClassBase
{
protected:
	AdapterItemClass();
public:
	virtual std::string GetString(void* data, Evas_Object *obj, const char *part) final;
	virtual Evas_Object* GetContent(void* data, Evas_Object *obj, const char *part) final;
	virtual void Deallocator(void* data);
	virtual std::string GetString(T* data, Evas_Object *obj, const char *part) = 0;
	virtual Evas_Object* GetContent(T* data, Evas_Object *obj, const char *part) = 0;
	virtual ~AdapterItemClass() { };
};

class LIBAPI Adapter
{
public:
	struct AdapterItem
	{
		void* data;
		AdapterItemClassBase* itemClass;
		Elm_Object_Item* objectItem;
	};

	Adapter();
	Adapter(Adapter&) = delete;
	~Adapter();

	template<class T>
	void AddItem(T* data, AdapterItemClass<T>* itemClass);

	template<class T>
	void RemoveItem(T* data);

	void Clear(bool preserve = false);
	std::list<AdapterItem>& GetAll();
	int GetCount();

	Event<Adapter*, AdapterItem*> OnItemAdd;
	Event<Adapter*, AdapterItem*> OnItemRemove;
protected:
	void AddItemInternal(void* data, AdapterItemClassBase* itemClass);
	void RemoveItemInternal(void* data);
private:
	std::list<AdapterItem> adapterItems;


};

class LIBAPI BasicListAdapter : public Adapter
{
public:
	struct BasicAdapterItem
	{
		int index;
		std::string value;
	};

	BasicListAdapter();
	BasicListAdapter(std::initializer_list<std::string> init);
	~BasicListAdapter();

	void AddItem(std::string string);
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
