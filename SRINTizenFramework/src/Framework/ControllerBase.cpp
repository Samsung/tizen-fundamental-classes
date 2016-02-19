/*
 * ControllerBase.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: gilang
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

LIBAPI ControllerBase::ControllerBase(ControllerManager* m, ViewBase* v, CString c) :
		Manager(m), View(v), ControllerName(c)
{
	Title = "";
}

LIBAPI void ControllerBase::OnLoad(void* data)
{
}

LIBAPI void* ControllerBase::OnUnload()
{
	return NULL;
}

LIBAPI void ControllerBase::Load(void* data)
{
	OnLoad(data);
}

LIBAPI void* ControllerBase::Unload()
{
	return OnUnload();
}

LIBAPI void ControllerBase::OnReload(void* data)
{
}

LIBAPI void ControllerBase::Reload(void* data)
{
	OnReload(data);
}

LIBAPI ControllerBase::~ControllerBase()
{
}



