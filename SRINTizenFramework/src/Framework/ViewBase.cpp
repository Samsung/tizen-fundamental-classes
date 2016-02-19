/*
 * ViewBase.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: gilang
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

LIBAPI ViewBase::ViewBase(CString viewName) :
		viewName(viewName)
{
	viewRoot = nullptr;
}
LIBAPI Evas_Object* ViewBase::Create(Evas_Object* root)
{
	if(!this->viewRoot)
		this->viewRoot = CreateView(root);

	return viewRoot;
}

LIBAPI CString ViewBase::GetStyle()
{
	return nullptr;
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
