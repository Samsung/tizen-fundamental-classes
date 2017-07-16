/*
 * WidgetBase.cpp
 *
 *  Created on: Jul 15, 2017
 *      Author: gilang
 */

#include "TFC/UI/WidgetBase.h"
#include <dlog.h>
using namespace TFC::UI;

LIBAPI
WidgetBase::operator Evas_Object*()
{
	return widgetRootInternal;
}

LIBAPI
WidgetBase::operator Evas_Object const*() const
{
	return widgetRootInternal;
}

LIBAPI
WidgetBase::WidgetBase(Evas_Object* widget) :
	widgetRoot(widgetRootInternal),
	widgetRootInternal(widget)
{
	evas_object_data_set(widget, "tfc_widget", this);
	evas_object_event_callback_add(widget, EVAS_CALLBACK_DEL, Deallocator, this);
	SetWeight(HintExpandAll);
	SetAlign(HintFillAll);
	evas_object_show(widget);
}

LIBAPI
void WidgetBase::SetEnabled(bool const& enabled)
{
	elm_object_disabled_set(this->widgetRootInternal, !enabled);
}

LIBAPI
bool WidgetBase::GetEnabled() const
{
	return !elm_object_disabled_get(this->widgetRootInternal);
}

LIBAPI
bool WidgetBase::GetVisible() const
{
	return evas_object_visible_get(this->widgetRootInternal);
}

LIBAPI
void WidgetBase::SetVisible(bool const& visible)
{
	if(visible)
		evas_object_show(this->widgetRootInternal);
	else
		evas_object_hide(this->widgetRootInternal);
}

void WidgetBase::Deallocator(void* userData, Evas_Object* src, Evas* evas, void* eventData)
{
	delete static_cast<WidgetBase*>(userData);
}

LIBAPI
WidgetBase::~WidgetBase()
{

}

LIBAPI
void TFC::UI::WidgetBase::SetWeight(Coordinate const& c)
{
	evas_object_size_hint_weight_set(this->widgetRootInternal, c.x, c.y);
}

LIBAPI
Coordinate TFC::UI::WidgetBase::GetWeight() const
{
	Coordinate ret;
	evas_object_size_hint_weight_get(this->widgetRootInternal, &ret.x, &ret.y);
	return ret;
}

LIBAPI
void TFC::UI::WidgetBase::SetAlign(Coordinate const& c)
{
	evas_object_size_hint_align_set(this->widgetRootInternal, c.x, c.y);
}

LIBAPI
Coordinate TFC::UI::WidgetBase::GetAlign() const
{
	Coordinate ret;
	evas_object_size_hint_align_get(this->widgetRootInternal, &ret.x, &ret.y);
	return ret;
}
