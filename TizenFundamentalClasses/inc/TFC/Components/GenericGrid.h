/*
 * GenericGrid.h
 *
 *  Created on: Feb 2, 2017
 *      Author: azzam
 */

#ifndef TFC_COMPONENTS_GENERICGRID_H_
#define TFC_COMPONENTS_GENERICGRID_H_


#include "TFC/Components/ComponentBase.h"
#include "TFC/Components/Adapter.h"

#include <list>
#include <unordered_map>

namespace TFC {
namespace Components {
	class GenericGrid;

	/**
	 * Specialized AdapterItemClassBase that wraps gengrid's Elm_GenGrid_Item_Class.
	 * Enables use of adapter pattern in GenericGrid, and simplify item callbacks with GetContent and GetString.
	 * @see Elm_Gen_Item_Class
	 */
	class LIBAPI GenericGridItemClassBase : public virtual AdapterItemClassBase
	{
	public:
		/**
		 * Helper to retrieve a package that holds the item's class, data, and GenericGrid it belongs to.
		 *
		 * @param gengrid GenericGrid that the item of this class belongs to.
		 * @param itemData Item's data.
		 *
		 * @return Struct that consist of the GenericGridItemClassBase, along with gengrid and itemData from parameter.
		 */
		void* operator()(GenericGrid* gengrid, void* itemData);

		/**
		 * Operator overloading that enables casting from GenericGridItemClassBase to Elm_Gengrid_Item_Class*.
		 */
		operator Elm_Gengrid_Item_Class*();

		/**
		 * Destructor for GenericGridItemClassBase.
		 */
		virtual ~GenericGridItemClassBase();

		/**
		 * Method to get whether the item is clickable or not.
		 */
		bool IsItemClickEnabled() const;
	protected:
		GenericGridItemClassBase(char const* styleName, bool defaultEventClick = true);
		bool itemClickEnabled;
	private:
		Elm_Gengrid_Item_Class* itemClass;
	};

	/**
	 * Class that enables template for GenericGridItemClassBase.
	 * Inherit this if you want to create custom GenericGrid item class beyond what's provided in SimpleGenericGridItemClass.
	 */
	template<class T>
	class GenericGridItemClass : public AdapterItemClass<T>, public GenericGridItemClassBase
	{
	public:
		/**
		 * Destructor for GenericGridItemClass.
		 */
		virtual ~GenericGridItemClass() { };
	protected:
		/**
		 * Constructor for GenericGridItemClass.
		 *
		 * @param styleName Name of the style that will be used.
		 */
		GenericGridItemClass(char const* styleName);
	};

	/**
	 * A simple template for GenericGridItemClass.
	 * Provides easy to use mapping for template argument members as text parts,
	 * so you don't have to use GetString manually.
	 * It's preferable to inherit this class to be used in gengrid's adapter,
	 * especially if your gengrid consists of mostly texts.
	 */
	template<class T>
	class SimpleGenericGridItemClass : public GenericGridItemClass<T>
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
	public:
		virtual std::string GetString(T* data, Evas_Object *obj, const char *part) final;

		/**
		 * Abstract method for providing Evas_Object representation from a part of a gengrid item.
		 *
		 * @param data Item's data.
		 * @param obj Parent Evas_Object.
		 * @param part Corresponding part's name.
		 *
		 * @return The Evas_Object representation.
		 */
		virtual Evas_Object* GetContent(T* data, Evas_Object *obj, const char *part);

		/**
		 * Destructor for SimpleGenericGridItemClass.
		 */
		virtual ~SimpleGenericGridItemClass() { };
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
		 * Constructor for SimpleGenericGridItemClass.
		 *
		 * @param styleName Name of the style that will be used.
		 */
		SimpleGenericGridItemClass(char const* styleName);
	private:
		std::unordered_map<std::string, Pointer> mappingMap;
	};

	/**
	 * Component that provides interactive & customizable grid UI.
	 * It acts as a wrapper for Elm_Gengrid, and provides support for features such as load more on end of grid,
	 * showing back to top indicator after certain amount of scroll, and longpressing items.
	 * It uses adapter pattern for appending and removing items.
	 */
	class LIBAPI GenericGrid :
		public ComponentBase,
		EventEmitterClass<GenericGrid>,
		PropertyClass<GenericGrid>
	{
		using EventEmitterClass<GenericGrid>::Event;
		using PropertyClass<GenericGrid>::Property;
	public:
		struct ItemSignalEventInfo;

		/**
		 * Constructor for GenericGrid.
		 */
		GenericGrid();

		~GenericGrid();


		/**
		 * Method to reset scroll position to top.
		 *
		 * @param animated If true, the resetting procedure will be animated.
		 */
		void ResetScroll(bool animated);

		/**
		 * Method to reset scroll position to top.
		 *
		 *  @param w The items' width.
		 *  @param h The items' height.
		 */
		//void SetItemSize(int w, int h);

		/**
		 * Event that will be triggered when the grid is scrolled.
		 */
		Event<void*> eventScrolled;

		/**
		 * Event that will be triggered when the grid is scrolled down.
		 */
		Event<void*> eventScrolledDown;

		/**
		 * Event that will be triggered when the grid is scrolled up.
		 */
		Event<void*> eventScrolledUp;

		/**
		 * Event that will be triggered when the grid is scrolled to the bottom of the grid.
		 */
		Event<void*> eventScrolledBottom;

		/**
		 * Event that will be triggered when the grid is scrolled to the top of the grid.
		 */
		Event<void*> eventScrolledTop;

		/**
		 * Event that will be triggered when the grid is scrolled to the left of the grid.
		 */
		Event<void*> eventScrolledLeft;

		/**
		 * Event that will be triggered when the grid is scrolled to the right of the grid.
		 */
		Event<void*> eventScrolledRight;

		/**
		 * Event that will be triggered when an item on the grid is clicked.
		 */
		Event<void*> eventItemClicked;

		/**
		 * Event that will be triggered when an item on the grid is longpressed.
		 */
		Event<void*> eventItemLongClicked;

		/**
		 * Event that will be triggered when the grid is starting to scroll.
		 */
		Event<void*> eventScrollingStart;

		/**
		 * Event that will be triggered when the grid is ending its scroll.
		 */
		Event<void*> eventScrollingEnd;

		Event<ItemSignalEventInfo> eventItemSignal;

	protected:
		/**
		 * Method overriden from ComponentBase, creates the UI elements of the component.
		 *
		 * @param root The root/parent given for this component.
		 *
		 * @return An Elm_Gengrid widget.
		 */
		virtual Evas_Object* CreateComponent(Evas_Object* root) override;

	private:
		Evas_Object* gengrid;
		Elm_Object_Item* firstItem;
		Elm_Object_Item* lastItem;
		Adapter* dataSource;
		bool longpressed;
		bool horizontalMode;
		bool isScrolling;
		int itemWidth;
		int itemHeight;

		void SetDataSource(Adapter* adapter);
		Adapter* GetDataSource();

		void SetLongClicked(bool const& o);
		bool GetLongClicked() const;

		void SetHorizontalMode(bool const& o);
		bool GetHorizontalMode() const;

		void SetWidth(int const& o);
		int GetWidth() const;

		void SetHeight(int const& o);
		int GetHeight() const;

		void AppendItemToGengrid(Adapter::AdapterItem* data);

		void OnItemAdd(
				Adapter* adapter,
				Adapter::AdapterItem* data);

		void OnItemRemove(
				Adapter* adapter,
				Adapter::AdapterItem* data);

		EvasSmartEvent eventScrolledInternal;
		EvasSmartEvent eventScrolledDownInternal;
		EvasSmartEvent eventScrolledUpInternal;
		EvasSmartEvent eventScrolledBottomInternal;
		EvasSmartEvent eventScrolledTopInternal;
		EvasSmartEvent eventScrolledLeftInternal;
		EvasSmartEvent eventScrolledRightInternal;
		EvasSmartEvent eventItemClickedInternal;
		EvasSmartEvent eventScrollingStartInternal;
		EvasSmartEvent eventScrollingEndInternal;
		EvasSmartEvent eventLongPressedInternal;
		ObjectItemEdjeSignalEvent eventItemSignalInternal;


		void OnScrolledInternal(Evas_Object* obj, void* eventData);
		void OnScrolledDownInternal(Evas_Object* obj, void* eventData);
		void OnScrolledUpInternal(Evas_Object* obj, void* eventData);
		void OnScrolledTopInternal(Evas_Object* obj, void* eventData);
		void OnScrolledBottomInternal(Evas_Object* obj, void* eventData);
		void OnScrolledLeftInternal(Evas_Object* obj, void* eventData);
		void OnScrolledRightInternal(Evas_Object* obj, void* eventData);
		void OnScrollingStart(Evas_Object* obj, void* eventData);
		void OnScrollingEnd(Evas_Object* obj, void* eventData);
		void OnLongPressedInternal(Evas_Object* obj, void* eventData);
		void OnItemSignalEmit(Elm_Object_Item* obj, EFL::EdjeSignalInfo eventData);


	public:
		// Property Declaration
		/**
		 * Property that enables getting & setting the adapter of gengrid.
		 * The return/parameter type is Adapter.
		 */
		Property<Adapter*>::Get<&GenericGrid::GetDataSource>::Set<&GenericGrid::SetDataSource> DataSource;


		/**
		 * Property that enables getting & setting whether longpressing item is enabled or not.
		 * The return/parameter type is bool.
		 */
		Property<bool>::Get<&GenericGrid::GetLongClicked>::Set<&GenericGrid::SetLongClicked> IsLongClicked;

		/**
		 * Property that enables getting & setting whether horizontalmode item is enabled or not.
		 * The return/parameter type is bool.
		 */
		Property<bool>::Get<&GenericGrid::GetHorizontalMode>::Set<&GenericGrid::SetHorizontalMode> IsHorizontalMode;

		/**
		 * Property that enables getting & setting width of item.
		 * The return/parameter type is int.
		 */
		Property<int>::Get<&GenericGrid::GetWidth>::Set<&GenericGrid::SetWidth> Width;

		/**
		 * Property that enables getting & setting height of item.
		 * The return/parameter type is int.
		 */
		Property<int>::Get<&GenericGrid::GetHeight>::Set<&GenericGrid::SetHeight> Height;

	};
}
}


struct TFC::Components::GenericGrid::ItemSignalEventInfo
{
	TFC::Components::Adapter::AdapterItem* adapterItem;
	TFC::EFL::EdjeSignalInfo edjeSignalInfo;
};


template<class T>
TFC::Components::GenericGridItemClass<T>::GenericGridItemClass(char const* styleName) : GenericGridItemClassBase(styleName)
{

}


template<class T>
TFC::Components::SimpleGenericGridItemClass<T>::SimpleGenericGridItemClass(char const* styleName) :
	GenericGridItemClass<T>(styleName)
{
}


template<class T>
std::string TFC::Components::SimpleGenericGridItemClass<T>::GetString(T* data, Evas_Object* obj,
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
void TFC::Components::SimpleGenericGridItemClass<T>::AddToMap(PointerToMemberString ptrMember, std::string mapTo)
{
	Pointer ptr;
	ptr.isFunction = false;
	ptr.ptrMember = ptrMember;
	mappingMap[mapTo] = ptr;
}

template<class T>
void TFC::Components::SimpleGenericGridItemClass<T>::AddToMap(PointerToGetterString ptrGetter, std::string mapTo)
{
	Pointer ptr;
	ptr.isFunction = true;
	ptr.ptrGetter = ptrGetter;
	mappingMap[mapTo] = ptr;
}

template<class T>
inline void TFC::Components::SimpleGenericGridItemClass<T>::AddToMap(PointerToGetterStringConst ptrGetter, std::string mapTo)
{
	Pointer ptr;
	ptr.isFunction = true;
	ptr.ptrGetter = reinterpret_cast<PointerToGetterString>(ptrGetter);
	mappingMap[mapTo] = ptr;
}

template<class T>
Evas_Object* TFC::Components::SimpleGenericGridItemClass<T>::GetContent(T* data, Evas_Object* obj,
	const char* part)
{
	return nullptr;
}


#endif /* TFC_COMPONENTS_GENERICGRID_H_ */
