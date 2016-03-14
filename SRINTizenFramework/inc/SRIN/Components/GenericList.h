/*
 * Adapter.h
 *
 *  Created on: Mar 8, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_ADAPTER_H_
#define SRIN_ADAPTER_H_

#include "SRIN/Components/ComponentBase.h"

#include <list>
#include <unordered_map>
#include "SRIN/Core.h"
#include "SRIN/ELMInterface.h"

namespace SRIN {
namespace Components {
	class GenericList;

	class LIBAPI GenericListItemClassBase
	{
	private:
		Elm_Genlist_Item_Class* itemClass;
	protected:
		GenericListItemClassBase(CString styleName);
	public:
		void* operator()(GenericList* genlist, void* itemData);
		operator Elm_Genlist_Item_Class*();
		virtual ~GenericListItemClassBase();

		virtual std::string GetString(void* data, Evas_Object* obj, const char* part) = 0;
		virtual Evas_Object* GetContent(void* data, Evas_Object* obj, const char* part) = 0;
		virtual void Deallocator(void* data) = 0;
	};

	template<class T>
	class GenericListItemClass : public GenericListItemClassBase
	{
	protected:
		GenericListItemClass(CString styleName);
	public:
		virtual std::string GetString(void* data, Evas_Object *obj, const char *part) final;
		virtual Evas_Object* GetContent(void* data, Evas_Object *obj, const char *part) final;
		virtual void Deallocator(void* data) final;
		virtual std::string GetString(T* data, Evas_Object *obj, const char *part) = 0;
		virtual Evas_Object* GetContent(T* data, Evas_Object *obj, const char *part) = 0;
		virtual ~GenericListItemClass() { };
	};

	template<class T>
	class SimpleGenericListItemClass : public GenericListItemClass<T>
	{
	private:
		typedef std::string T::* PointerToMemberString;
		typedef const std::string& (T::* PointerToGetterString)();

		struct Pointer {
			bool isFunction;
			union {
				PointerToMemberString ptrMember;
				PointerToGetterString ptrGetter;
			};
		};

		std::unordered_map<std::string, Pointer> mappingMap;
	protected:
		void AddToMap(PointerToMemberString ptr, std::string mapTo);
		void AddToMap(PointerToGetterString ptr, std::string mapTo);
		SimpleGenericListItemClass(CString styleName);
	public:
		virtual std::string GetString(T* data, Evas_Object *obj, const char *part) final;
		virtual Evas_Object* GetContent(T* data, Evas_Object *obj, const char *part);
		virtual ~SimpleGenericListItemClass() { };
	};

	class LIBAPI GenericListAdapter
	{
	private:
		struct GenlistItem
		{
			void* data;
			GenericListItemClassBase* itemClass;
			Elm_Object_Item* objectItem;
		};
		int count;
		std::list<GenlistItem> adapterItems;
		Event<GenericListAdapter*, GenlistItem*> OnItemAdd;
		Event<GenericListAdapter*, GenlistItem*> OnItemRemove;
	protected:
		void AddItemInternal(void* data, GenericListItemClassBase* itemClass);
		void RemoveItemInternal(void* data);

		std::list<GenlistItem>& GetAll();
	public:
		GenericListAdapter();
		GenericListAdapter(GenericListAdapter&) = delete;
		~GenericListAdapter();

		template<class T>
		void AddItem(T* data, GenericListItemClass<T>* itemClass);

		template<class T>
		void RemoveItem(T* data);

		void Clear(bool preserve = false);

		int GetCount();

		friend class GenericList;
	};


	class LIBAPI GenericList : public ComponentBase
	{
	private:
		Evas_Object* genlist;
		Elm_Object_Item* dummyBottom;
		Elm_Object_Item* realBottom;
		Elm_Genlist_Item_Class* dummyBottomItemClass;
		GenericListAdapter* adapter;
		bool overscroll;

		void SetAdapter(GenericListAdapter* const & adapter);
		GenericListAdapter*& GetAdapter();

		void SetOverscroll(bool const& o);
		bool& GetOverscroll();

		void AppendItemToGenlist(GenericListAdapter::GenlistItem* data);
		void OnItemAdd(Event<GenericListAdapter*, GenericListAdapter::GenlistItem*>* event, GenericListAdapter* adapter, GenericListAdapter::GenlistItem* data);
		void OnItemRemove(Event<GenericListAdapter*, GenericListAdapter::GenlistItem*>* event, GenericListAdapter* adapter, GenericListAdapter::GenlistItem* data);

		ElementaryEvent onScrolledBottomInternal;
		ElementaryEvent onScrolledTopInternal;
		ElementaryEvent onDummyRealized;
		ElementaryEvent onItemClickedInternal;


		void OnScrolledBottomInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnScrolledTopInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnDummyRealized(ElementaryEvent* event, Evas_Object* obj, void* eventData);
	protected:
		virtual Evas_Object* CreateComponent(Evas_Object* root);
	public:
		GenericList();
		Property<GenericList, GenericListAdapter*, &GenericList::GetAdapter, &GenericList::SetAdapter> Adapter;
		Property<GenericList, bool, &GenericList::GetOverscroll, &GenericList::SetOverscroll> Overscroll;
		Event<GenericList*, void*> OnScrolledBottom;
		Event<GenericList*, void*> OnScrolledTop;
		Event<GenericList*, void*> OnReachingBottom;
		Event<GenericList*, void*> OnItemClicked;
	};
}
}

template<class T>
SRIN::Components::GenericListItemClass<T>::GenericListItemClass(CString styleName) : GenericListItemClassBase(styleName)
{

}

template<class T>
void SRIN::Components::GenericListAdapter::AddItem(T* data, GenericListItemClass<T>* itemClass)
{
	AddItemInternal(data, itemClass);
}

template<class T>
void SRIN::Components::GenericListAdapter::RemoveItem(T* data)
{
	RemoveItemInternal(data);
}

template<class T>
SRIN::Components::SimpleGenericListItemClass<T>::SimpleGenericListItemClass(CString styleName) :
	GenericListItemClass<T>(styleName)
{
}


template<class T>
std::string SRIN::Components::GenericListItemClass<T>::GetString(void* data, Evas_Object* obj, const char* part)
{
	return this->GetString(reinterpret_cast<T*>(data), obj, part);
}

template<class T>
Evas_Object* SRIN::Components::GenericListItemClass<T>::GetContent(void* data, Evas_Object* obj, const char* part)
{
	return this->GetContent(reinterpret_cast<T*>(data), obj, part);
}

template<class T>
void SRIN::Components::GenericListItemClass<T>::Deallocator(void* data)
{
	auto obj = reinterpret_cast<T*>(data);
	delete obj;
}

template<class T>
std::string SRIN::Components::SimpleGenericListItemClass<T>::GetString(T* data, Evas_Object* obj,
	const char* part)
{
	auto elem = mappingMap.find(part);
	if(elem != mappingMap.end())
	{
		auto& ptr = elem->second;
		if(!ptr.isFunction)
			return (data->*ptr.ptrMember);
		else
			return (data->*ptr.ptrGetter)();
	}
	else
		return "";
}

template<class T>
void SRIN::Components::SimpleGenericListItemClass<T>::AddToMap(PointerToMemberString ptrMember, std::string mapTo)
{
	Pointer ptr;
	ptr.isFunction = false;
	ptr.ptrMember = ptrMember;
	mappingMap[mapTo] = ptr;
}

template<class T>
void SRIN::Components::SimpleGenericListItemClass<T>::AddToMap(PointerToGetterString ptrGetter, std::string mapTo)
{
	Pointer ptr;
	ptr.isFunction = true;
	ptr.ptrGetter = ptrGetter;
	mappingMap[mapTo] = ptr;
}

template<class T>
Evas_Object* SRIN::Components::SimpleGenericListItemClass<T>::GetContent(T* data, Evas_Object* obj,
	const char* part)
{
	return nullptr;
}



#endif /* ADAPTER_H_ */
