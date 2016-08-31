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

namespace SRIN {
namespace Components {
	class GenericList;

	class LIBAPI GenericListItemClassBase : public virtual AdapterItemClassBase
	{
	private:
		Elm_Genlist_Item_Class* itemClass;
	protected:
		GenericListItemClassBase(CString styleName, bool defaultEventClick = true);
		bool itemClickEnabled;
	public:
		void* operator()(GenericList* genlist, void* itemData);
		operator Elm_Genlist_Item_Class*();
		virtual ~GenericListItemClassBase();
		bool IsItemClickEnabled();
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
		typedef const std::string& (T::* PointerToGetterStringConst)() const;

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
		void AddToMap(PointerToGetterStringConst ptr, std::string mapTo);
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
		Elm_Object_Item* dummyTop;
		Elm_Object_Item* realBottom;
		Elm_Genlist_Item_Class* dummyBottomItemClass;
		Elm_Genlist_Item_Class* dummyTopItemClass;
		Adapter* dataSource;
		bool overscroll;
		bool underscroll;
		bool longpressed;
		bool isScrolling;
		int backToTopThreshold;
		bool backToTopShown;
		bool firstRealize;

		void SetDataSource(Adapter* adapter);
		Adapter* GetDataSource();

		void SetOverscroll(bool const& o);
		bool GetOverscroll();

		void SetUnderscroll(bool const& o);
		bool GetUnderscroll();

		void SetLongClicked(bool const& o);
		bool GetLongClicked();

		void SetBackToTopThreshold(int const& o);
		int GetBackToTopThreshold();

		void AppendItemToGenlist(Adapter::AdapterItem* data);
		void OnItemAdd(Event<Adapter*, Adapter::AdapterItem*>* event, Adapter* adapter, Adapter::AdapterItem* data);
		void OnItemRemove(Event<Adapter*, Adapter::AdapterItem*>* event, Adapter* adapter, Adapter::AdapterItem* data);

		EFL::EvasSmartEvent eventScrolledInternal;
		EFL::EvasSmartEvent eventScrolledDownInternal;
		EFL::EvasSmartEvent eventScrolledUpInternal;
		EFL::EvasSmartEvent eventScrolledBottomInternal;
		EFL::EvasSmartEvent eventScrolledTopInternal;
		EFL::EvasSmartEvent eventDummyRealized;
		EFL::EvasSmartEvent eventItemClickedInternal;
		EFL::EvasSmartEvent eventScrollingStartInternal;
		EFL::EvasSmartEvent eventScrollingEndInternal;
		EFL::EvasSmartEvent eventLongPressedInternal;
		EFL::EvasSmartEvent eventItemUnrealized;
		EFL::ObjectItemEdjeSignalEvent eventItemSignalInternal;

		
		void OnScrolledInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData);
		void OnScrolledDownInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData);
		void OnScrolledUpInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData);
		void OnScrolledBottomInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData);
		void OnScrolledTopInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData);
		void OnDummyRealized(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData);
		void OnScrollingStart(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData);
		void OnScrollingEnd(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData);
		void OnLongPressedInternal(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData);
		void OnItemSignalEmit(EFL::ObjectItemEdjeSignalEvent* event, Elm_Object_Item* obj, EFL::EdjeSignalInfo eventData);
		void OnItemUnrealized(EFL::EvasSmartEvent* event, Evas_Object* obj, void* eventData);
		
	protected:
		virtual Evas_Object* CreateComponent(Evas_Object* root);
	public:
		GenericList();
		~GenericList();
		void ResetScroll(bool animated);
		Property<GenericList, Adapter*>::GetSet<&GenericList::GetDataSource, &GenericList::SetDataSource> DataSource;
		Property<GenericList, bool>::GetSet<&GenericList::GetOverscroll, &GenericList::SetOverscroll> Overscroll;
		Property<GenericList, bool>::GetSet<&GenericList::GetUnderscroll, &GenericList::SetUnderscroll> Underscroll;
		Property<GenericList, bool>::GetSet<&GenericList::GetLongClicked, &GenericList::SetLongClicked> IsLongClicked;
		Property<GenericList, int>::GetSet<&GenericList::GetBackToTopThreshold, &GenericList::SetBackToTopThreshold> BackToTopThreshold;

		Event<GenericList*, void*> eventScrolled;
		Event<GenericList*, void*> eventScrolledDown;
		Event<GenericList*, void*> eventScrolledUp;
		Event<GenericList*, void*> eventScrolledBottom;
		Event<GenericList*, void*> eventScrolledTop;
		Event<GenericList*, void*> eventReachingBottom;
		Event<GenericList*, void*> eventItemClicked;
		Event<GenericList*, void*> eventItemLongClicked;
		Event<GenericList*, void*> eventScrollingStart;
		Event<GenericList*, void*> eventScrollingEnd;
		Event<GenericList*, bool*> eventShowBackToTop;
		Event<GenericList*, Evas_Object**> eventDummyBottomContent;
		Event<Adapter::AdapterItem*, EFL::EdjeSignalInfo> eventItemSignal;

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
inline void SRIN::Components::SimpleGenericListItemClass<T>::AddToMap(PointerToGetterStringConst ptrGetter, std::string mapTo)
{
	Pointer ptr;
	ptr.isFunction = true;
	ptr.ptrGetter = reinterpret_cast<PointerToGetterString>(ptrGetter);
	mappingMap[mapTo] = ptr;
}

template<class T>
Evas_Object* SRIN::Components::SimpleGenericListItemClass<T>::GetContent(T* data, Evas_Object* obj,
	const char* part)
{
	return nullptr;
}



#endif /* ADAPTER_H_ */
