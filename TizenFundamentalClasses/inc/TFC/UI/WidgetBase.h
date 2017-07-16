/*
 * WidgetBase.h
 *
 *  Created on: Jul 15, 2017
 *      Author: gilang
 */

#ifndef TFC_UI_WIDGETBASE_H_
#define TFC_UI_WIDGETBASE_H_

#include "TFC/Core.h"
#include "TFC/EFL.h"
#include <string>

namespace TFC {
namespace UI {

	struct Coordinate {
		double x;
		double y;
	};

	static constexpr Coordinate HintFillAll { EVAS_HINT_FILL, EVAS_HINT_FILL };
	static constexpr Coordinate HintExpandAll { EVAS_HINT_EXPAND, EVAS_HINT_EXPAND };

	class WidgetBase :
			EventEmitterClass<WidgetBase>,
			public EFL::EFLProxyClass,
			public PropertyClass<WidgetBase>,
			public ManagedClass
	{
	public:
		operator Evas_Object*();
		operator Evas_Object const*() const;

		TFC_Property(WidgetBase, bool, Enabled);
		TFC_Property(WidgetBase, bool, Visible);
		TFC_Property(WidgetBase, Coordinate, Weight);
		TFC_Property(WidgetBase, Coordinate, Align);

	protected:
		WidgetBase(Evas_Object* widget);
		Evas_Object* const& widgetRoot;
		virtual ~WidgetBase();

	private:
		static void Deallocator(void* userData, Evas_Object* src, Evas* evas, void* eventData);
		Evas_Object* widgetRootInternal;
	};

}}

#endif /* TFC_UI_WIDGETBASE_H_ */
