/*
 * ControllerBase.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: gilang
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

ControllerBase::ControllerBase(ControllerManager* m, ViewBase* v, CString c) :
		Manager(m), ControllerName(c), View(v)
{
	Title = "";
}

void ControllerBase::OnLoad(void* data)
{
}

void* ControllerBase::OnUnload()
{
	return NULL;
}

void ControllerBase::Load(void* data)
{
	OnLoad(data);
}

void* ControllerBase::Unload()
{
	return OnUnload();
}

void ControllerBase::OnReload(void* data)
{
}

void ControllerBase::Reload(void* data)
{
	OnReload(data);
}

ControllerBase::~ControllerBase()
{
}



