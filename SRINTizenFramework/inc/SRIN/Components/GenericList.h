/*
 * Adapter.h
 *
 *  Created on: Mar 8, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_ADAPTER_H_
#define SRIN_ADAPTER_H_

#include "SRIN/Components/ComponentBase.h"
#include "SRIN/Components/Adapter.h"

#include <list>
#include <unordered_map>
#include "SRIN/Core.h"
#include "SRIN/ELMInterface.h"

namespace SRIN {
namespace Components {
	class GenericList;

	class LIBAPI GenericListItemClassBase : public virtual AdapterItemClassBase
	{
	private:
		Elm_Genlist_Item_Class* itemClass;
	protected:
		GenericListItemClassBase(CString styleName);
	public:
		void* operator()(GenericList* genlist, void* itemData);
		operator Elm_Genlist_Item_Class*();
		virtual ~GenericListItemClassBase();
	};

	template<class T>
	class GenericListItemClass : public AdapterItemClass<T>, public GenericListItemClassBase
	{
	protected:
		GenericListItemClass(CString styleName);
	public:
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

	class LIBAPI GenericList : public ComponentBase
	{
	private:
		Evas_Object* genlist;
		Elm_Object_Item* dummyBottom;
		Elm_Object_Item* realBottom;
		Elm_Genlist_Item_Class* dummyBottomItemClass;
		Adapter* dataSource;
		bool overscroll;

		void SetDataSource(Adapter* adapter);
		Adapter* GetDataSource();

		void SetOverscroll(bool const& o);
		bool GetOverscroll();

		void AppendItemToGenlist(Adapter::AdapterItem* data);
		void OnItemAdd(Event<Adapter*, Adapter::AdapterItem*>* event, Adapter* adapter, Adapter::AdapterItem* data);
		void OnItemRemove(Event<Adapter*, Adapter::AdapterItem*>* event, Adapter* adapter, Adapter::AdapterItem* data);

		ElementaryEvent onScrolledBottomInternal;
		ElementaryEvent onScrolledTopInternal;
		ElementaryEvent onDummyRealized;
		ElementaryEvent onItemClickedInternal;
		ElementaryEvent onScrollingStart;
		ElementaryEvent onScrollingEnd;


		void OnScrolledBottomInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnScrolledTopInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnDummyRealized(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnScrollingStart(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnScrollingEnd(ElementaryEvent* event, Evas_Object* obj, void* eventData);
	protected:
		virtual Evas_Object* CreateComponent(Evas_Object* root);
	public:
		GenericList();
		Property<GenericList, Adapter*>::GetSet<&GenericList::GetDataSource, &GenericList::SetDataSource> DataSource;
		Property<GenericList, bool>::GetSet<&GenericList::GetOverscroll, &GenericList::SetOverscroll> Overscroll;
		Event<GenericList*, void*> ScrolledBottom;
		Event<GenericList*, void*> ScrolledTop;
		Event<GenericList*, void*> ReachingBottom;
		Event<GenericList*, void*> ItemClicked;
		Event<GenericList*, void*> ScrollingStart;
		Event<GenericList*, void*> ScrollingEnd;
	};
}
}

template<class T>
SRIN::Components::GenericListItemClass<T>::GenericListItemClass(CString styleName) : GenericListItemClassBase(styleName)
{

}


template<class T>
SRIN::Components::SimpleGenericListItemClass<T>::SimpleGenericListItemClass(CString styleName) :
	GenericListItemClass<T>(styleName)
{
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
