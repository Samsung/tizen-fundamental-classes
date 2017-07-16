/*
 * Widget.h
 *
 *  Created on: Jul 15, 2017
 *      Author: gilang
 */

#ifndef TFC_UI_WIDGET_H_
#define TFC_UI_WIDGET_H_

#include "TFC/Core.h"
#include "TFC/Core/Metaprogramming.h"
#include "TFC/UI/WidgetBase.h"

namespace TFC {
namespace UI {

	template<typename TWidget, typename = typename std::is_base_of<TFC::UI::WidgetBase, TWidget>::type>
	class Widget
	{
	public:


		Widget() { }
		Widget(Widget const& other) = default;
		Widget(Widget&& other) :
			theWidget(other.theWidget),
			safeHandle(std::move(other.safeHandle))
		{
			other.theWidget = nullptr;
		}

		Widget& operator=(Widget const& other) = default;
		Widget& operator=(Widget&& other)
		{
			theWidget = other.theWidget;
			safeHandle = std::move(other.safeHandle);
			other.theWidget = nullptr;

			return *this;
		}

		template<typename TWidgetIn, typename = typename std::is_base_of<TFC::UI::WidgetBase, TWidgetIn>::type>
		Widget& operator=(TWidgetIn* newWidget)
		{
			theWidget = newWidget;
			safeHandle = newWidget->GetSafePointer();
			return *this;
		}

		TWidget* operator->()
		{
			safeHandle.ThrowIfUnsafe();
			return theWidget;
		}

		TWidget const* operator->() const
		{
			safeHandle.ThrowIfUnsafe();
			return theWidget;
		}

		template<typename TValue>
		TValue& operator->*(TValue TWidget::* tPtr)
		{
			safeHandle.ThrowIfUnsafe();
			return theWidget->*tPtr;
		}

		operator Evas_Object*()
		{
			return static_cast<Evas_Object*>(*theWidget);
		}

		static void* operator new(std::size_t size) = delete;
		static void* operator new[](std::size_t size) = delete;

	private:
		TWidget* theWidget { nullptr };
		ManagedClass::SafePointer safeHandle;
	};

	template<typename TWidget, typename TEventType, TEventType TWidget::* eventPtr>
	class WidgetEventObject
	{
	public:
		WidgetEventObject(Widget<TWidget>& w) :
			w(w)
		{ }

		template<typename THandler>
		void operator+=(THandler other)
		{
			(w->*eventPtr).operator+=(std::forward<THandler>(other));
		}

		template<typename THandler>
		void operator-=(THandler other)
		{
			(w->*eventPtr).operator-=(std::forward<THandler>(other));
		}

	private:
		Widget<TWidget>& w;
	};

	template<typename TWidget,
			 typename TPropertyType,
			 TPropertyType TWidget::* propertyPtr>
	class WidgetPropertyObject
	{
	private:
		Widget<TWidget>& w;

	public:
		typedef typename TPropertyType::ValueType ValueType;

		WidgetPropertyObject(Widget<TWidget>& w) :
			w(w)
		{ }

		operator ValueType()
		{
			return (w->*propertyPtr).operator ValueType();
		}

		auto operator->()
		{
			return (w->*propertyPtr).operator->();
		}

		template<typename TSet>
		void operator=(TSet val)
		{
			(w->*propertyPtr).operator=(std::forward<TSet>(val));
		}


	};

}}

#define TFC_WidgetEventForward(EVENT_PTR) TFC::UI::WidgetEventObject< \
				typename TFC::Core::Introspect::MemberField<decltype(&EVENT_PTR)>::DeclaringType,\
				typename TFC::Core::Introspect::MemberField<decltype(&EVENT_PTR)>::ValueType,\
				&EVENT_PTR>

#define TFC_WidgetPropertyForward(PROPERTY_PTR) TFC::UI::WidgetPropertyObject< \
				typename TFC::Core::Introspect::MemberField<decltype(&PROPERTY_PTR)>::DeclaringType,\
				typename TFC::Core::Introspect::MemberField<decltype(&PROPERTY_PTR)>::ValueType,\
				&PROPERTY_PTR>

#endif /* TFC_UI_WIDGET_H_ */
