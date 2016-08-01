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
		Elm_Object_Item* realBottom;
		Elm_Genlist_Item_Class* dummyBottomItemClass;
		Adapter* dataSource;
		bool overscroll;
		bool longpressed;
		bool isScrolling;
		int backToTopThreshold;
		bool backToTopShown;

		void SetDataSource(Adapter* adapter);
		Adapter* GetDataSource();

		void SetOverscroll(bool const& o);
		bool GetOverscroll();

		void SetLongClicked(bool const& o);
		bool GetLongClicked();

		void SetBackToTopThreshold(int const& o);
		int GetBackToTopThreshold();

		void AppendItemToGenlist(Adapter::AdapterItem* data);
		void OnItemAdd(Event<Adapter*, Adapter::AdapterItem*>* event, Adapter* adapter, Adapter::AdapterItem* data);
		void OnItemRemove(Event<Adapter*, Adapter::AdapterItem*>* event, Adapter* adapter, Adapter::AdapterItem* data);

		ElementaryEvent onScrolledInternal;
		ElementaryEvent onScrolledDownInternal;
		ElementaryEvent onScrolledUpInternal;
		ElementaryEvent onScrolledBottomInternal;
		ElementaryEvent onScrolledTopInternal;
		ElementaryEvent onDummyRealized;
		ElementaryEvent onItemClickedInternal;
		ElementaryEvent onScrollingStart;
		ElementaryEvent onScrollingEnd;
		ElementaryEvent onLongPressedInternal;
		ObjectItemEdjeSignalEvent ItemSignalInternal;

		void OnScrolledInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnScrolledDownInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnScrolledUpInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnScrolledBottomInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnScrolledTopInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnDummyRealized(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnScrollingStart(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnScrollingEnd(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnLongPressedInternal(ElementaryEvent* event, Evas_Object* obj, void* eventData);
		void OnItemSignalEmit(ObjectItemEdjeSignalEvent* event, Elm_Object_Item* obj, EdjeSignalInfo eventData);
	protected:
		virtual Evas_Object* CreateComponent(Evas_Object* root);
	public:
		GenericList();
		void ResetScroll(bool animated);
		Property<GenericList, Adapter*>::GetSet<&GenericList::GetDataSource, &GenericList::SetDataSource> DataSource;
		Property<GenericList, bool>::GetSet<&GenericList::GetOverscroll, &GenericList::SetOverscroll> Overscroll;
		Property<GenericList, bool>::GetSet<&GenericList::GetLongClicked, &GenericList::SetLongClicked> IsLongClicked;
		Property<GenericList, int>::GetSet<&GenericList::GetBackToTopThreshold, &GenericList::SetBackToTopThreshold> BackToTopThreshold;
		Event<GenericList*, void*> Scrolled;
		Event<GenericList*, void*> ScrolledDown;
		Event<GenericList*, void*> ScrolledUp;
		Event<GenericList*, void*> ScrolledBottom;
		Event<GenericList*, void*> ScrolledTop;
		Event<GenericList*, void*> ReachingBottom;
		Event<GenericList*, void*> ItemClicked;
		Event<GenericList*, void*> ItemLongClicked;
		Event<GenericList*, void*> ScrollingStart;
		Event<GenericList*, void*> ScrollingEnd;
		Event<GenericList*, bool*> ShowBackToTop;
		Event<GenericList*, Evas_Object**> DummyBottomContent;
		Event<Adapter::AdapterItem*, EdjeSignalInfo> ItemSignal;

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
