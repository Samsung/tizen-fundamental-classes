/*
 * ViewBase.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

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

void SRIN::Framework::ITitleProvider::SetTitle(const std::string& value)
{
	this->viewTitle = value;
	dlog_print(DLOG_DEBUG, LOG_TAG, "Set title on ViewBase %s", value.c_str());
}

std::string& SRIN::Framework::ITitleProvider::GetTitle()
{
	return this->viewTitle;
}
