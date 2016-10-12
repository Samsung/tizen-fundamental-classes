/*
 * ViewBase.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: gilang
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

ViewBase::ViewBase(CString viewName) :
		viewName(viewName)
{
	viewRoot = nullptr;
}
Evas_Object* ViewBase::Create(Evas_Object* root)
{
	if(!this->viewRoot)
		this->viewRoot = CreateView(root);

	return viewRoot;
}

CString ViewBase::GetStyle()
{
	return nullptr;
}

bool ViewBase::IsCreated()
{
	return viewRoot != nullptr;
}

Evas_Object* ViewBase::GetViewRoot()
{
	return viewRoot;
}

ViewBase::~ViewBase()
{

}
