/*
 * ViewBase.cpp
 *
 *  Created on: Feb 15, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include <dlog.h>
#include "TFC/Framework/Application.h"

using namespace TFC::Framework;

LIBAPI ViewBase::ViewBase()
{
	viewRoot = nullptr;
}
LIBAPI Evas_Object* ViewBase::Create(Evas_Object* root)
{
	if (!this->viewRoot)
		this->viewRoot = CreateView(root);

	return viewRoot;
}


LIBAPI bool ViewBase::IsCreated()
{
	return viewRoot != nullptr;
}

LIBAPI Evas_Object* ViewBase::GetViewRoot()
{
	return viewRoot;
}

LIBAPI ViewBase::~ViewBase()
{

}

void TFC::Framework::ITitleProvider::SetTitle(const std::string& value)
{
	this->viewTitle = value;
	dlog_print(DLOG_DEBUG, LOG_TAG, "Set title on ViewBase %s", value.c_str());
}

std::string const& TFC::Framework::ITitleProvider::GetTitle() const
{
	return this->viewTitle;
}
