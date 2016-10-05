/*
 * Adapter.h
 *
 *  Created on: Mar 8, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        ib.putu (ib.putu@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
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

	/**
	 * Specialized AdapterItemClassBase that wraps genlist's Elm_Genlist_Item_Class.
	 * Enables use of adapter pattern in GenericList, and simplify item callbacks with GetContent and GetString.
	 * @see Elm_Gen_Item_Class
	 */
	class LIBAPI GenericListItemClassBase : public virtual AdapterItemClassBase
	{
	private:
		Elm_Genlist_Item_Class* itemClass;
	protected:
		GenericListItemClassBase(CString styleName, bool defaultEventClick = true);
		bool itemClickEnabled;
	public:
		/**
		 * Helper to retrieve a package that holds the item's class, data, and GenericList it belongs to.
		 *
		 * @param genlist GenericList that the item of this class belongs to.
		 * @param itemData Item's data.
		 *
		 * @return Struct that consist of the GenericListItemClassBase, along with genlist and itemData from parameter.
		 */
		void* operator()(GenericList* genlist, void* itemData);

		/**
		 * Operator overloading that enables casting from GenericListItemClassBase to Elm_Genlist_Item_Class*.
		 */
		operator Elm_Genlist_Item_Class*();

		/**
		 * Destructor for GenericListItemClassBase.
		 */
		virtual ~GenericListItemClassBase();

		/**
		 * Method to get whether the item is clickable or not.
		 */
		bool IsItemClickEnabled();
	};

	/**
	 * Class that enables template for GenericListItemClassBase.
	 * Inherit this if you want to create custom GenericList item class beyond what's provided in SimpleGenericListItemClass.
	 */
	template<class T>
	class GenericListItemClass : public AdapterItemClass<T>, public GenericListItemClassBase
	{
	protected:
		/**
		 * Constructor for GenericListItemClass.
		 *
		 * @param styleName Name of the style that will be used.
		 */
		GenericListItemClass(CString styleName);
	public:
		/**
		 * Destructor for GenericListItemClass.
		 */
		virtual ~GenericListItemClass() { };
	};

	/**
	 * A simple template for GenericListItemClass.
	 * Provides easy to use mapping for template argument members as text parts,
	 * so you don't have to use GetString manually.
	 * It's preferable to inherit this class to be used in genlist's adapter,
	 * especially if your genlist consists of mostly texts.
	 */
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
		/**
		 * Add mapping from a member string of the template argument to a certain part.
		 *
		 * @param ptr String member pointer (ex. &T::member)
		 * @param mapTo Corresponding part name where it will be placed.
		 */
		void AddToMap(PointerToMemberString ptr, std::string mapTo);

		/**
		 * Add mapping from a non-const getter string method of the template argument to a certain part.
		 *
		 * @param ptr String getter string method pointer (ex. &T::GetMember)
		 * @param mapTo Corresponding part name where it will be placed.
		 */
		void AddToMap(PointerToGetterString ptr, std::string mapTo);

		/**
		 * Add mapping from a const getter string method of the template argument to a certain part.
		 *
		 * @param ptr String getter string method pointer (ex. &T::GetMember)
		 * @param mapTo Corresponding part name where it will be placed.
		 */
		void AddToMap(PointerToGetterStringConst ptr, std::string mapTo);

		/**
		 * Constructor for SimpleGenericListItemClass.
		 *
		 * @param styleName Name of the style that will be used.
		 */
		SimpleGenericListItemClass(CString styleName);
	public:
		virtual std::string GetString(T* data, Evas_Object *obj, const char *part) final;

		/**
		 * Abstract method for providing Evas_Object representation from a part of a genlist item.
		 *
		 * @param data Item's data.
		 * @param obj Parent Evas_Object.
		 * @param part Corresponding part's name.
		 *
		 * @return The Evas_Object representation.
		 */
		virtual Evas_Object* GetContent(T* data, Evas_Object *obj, const char *part);

		/**
		 * Destructor for SimpleGenericListItemClass.
		 */
		virtual ~SimpleGenericListItemClass() { };
	};

	/**
	 * Component that provides interactive & customizable list UI.
	 * It acts as a wrapper for Elm_Genlist, and provides support for features such as load more on end of list,
	 * showing back to top indicator after certain amount of scroll, and longpressing items.
	 * It uses adapter pattern for appending and removing items.
	 */
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
		/**
		 * Method overriden from ComponentBase, creates the UI elements of the component.
		 *
		 * @param root The root/parent given for this component.
		 *
		 * @return An Elm_Genlist widget.
		 */
		virtual Evas_Object* CreateComponent(Evas_Object* root);
	public:
		/**
		 * Constructor for GenericList.
		 */
		GenericList();

		~GenericList();


		/**
		 * Method to reset scroll position to top.
		 *
		 * @param animated If true, the resetting procedure will be animated.
		 */
		void ResetScroll(bool animated);

		/**
		 * Property that enables getting & setting the adapter of genlist.
		 * The return/parameter type is Adapter.
		 */
		Property<GenericList, Adapter*>::GetSet<&GenericList::GetDataSource, &GenericList::SetDataSource> DataSource;

		/**
		 * Property that enables getting & setting whether overscrolling (end of list action) is enabled or not.
		 * The return/parameter type is bool.
		 */
		Property<GenericList, bool>::GetSet<&GenericList::GetOverscroll, &GenericList::SetOverscroll> Overscroll;

		Property<GenericList, bool>::GetSet<&GenericList::GetUnderscroll, &GenericList::SetUnderscroll> Underscroll;


		/**
		 * Property that enables getting & setting whether longpressing item is enabled or not.
		 * The return/parameter type is bool.
		 */
		Property<GenericList, bool>::GetSet<&GenericList::GetLongClicked, &GenericList::SetLongClicked> IsLongClicked;

		/**
		 * Property that enables getting & setting the threshold for showing back to top action.
		 * The return/parameter type is int.
		 */
		Property<GenericList, int>::GetSet<&GenericList::GetBackToTopThreshold, &GenericList::SetBackToTopThreshold> BackToTopThreshold;

		/**
		 * Event that will be triggered when the list is scrolled.
		 */
		Event<GenericList*, void*> eventScrolled;

		/**
		 * Event that will be triggered when the list is scrolled down.
		 */
		Event<GenericList*, void*> eventScrolledDown;

		/**
		 * Event that will be triggered when the list is scrolled up.
		 */
		Event<GenericList*, void*> eventScrolledUp;

		/**
		 * Event that will be triggered when the list is scrolled to the bottom of the list.
		 */
		Event<GenericList*, void*> eventScrolledBottom;

		/**
		 * Event that will be triggered when the list is scrolled to the top of the list.
		 */
		Event<GenericList*, void*> eventScrolledTop;

		/**
		 * Event that will be triggered when the dummy bottom-most item on the list is realized.
		 */
		Event<GenericList*, void*> eventReachingBottom;

		/**
		 * Event that will be triggered when an item on the list is clicked.
		 */
		Event<GenericList*, void*> eventItemClicked;

		/**
		 * Event that will be triggered when an item on the list is longpressed.
		 */
		Event<GenericList*, void*> eventItemLongClicked;

		/**
		 * Event that will be triggered when the list is starting to scroll.
		 */
		Event<GenericList*, void*> eventScrollingStart;

		/**
		 * Event that will be triggered when the list is ending its scroll.
		 */
		Event<GenericList*, void*> eventScrollingEnd;

		/**
		 * Event that will be triggered when the back to top scroll threshold is passed (so it will be appropriate to enable the back to top action).
		 */
		Event<GenericList*, bool*> eventShowBackToTop;

		/**
		 * Event that can be used to customize the dummy bottom-most item on the list by assigning an Evas_Object to its event data.
		 */
		Event<GenericList*, Evas_Object**> eventDummyBottomContent;

		Event<Adapter::AdapterItem*, EdjeSignalInfo> eventItemSignal;
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
