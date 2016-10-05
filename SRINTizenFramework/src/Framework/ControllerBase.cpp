/*
 * ControllerBase.cpp
 *
 *  Created on: Feb 15, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "SRIN/Framework/Application.h"

using namespace SRIN::Framework;

LIBAPI ControllerBase::ControllerBase(ControllerManager* m, ViewBase* v, CString c) :
	Manager(m), View(v), ControllerName(c)
{

}

LIBAPI void ControllerBase::OnLoad(ObjectClass* data)
{
}

LIBAPI ObjectClass* ControllerBase::OnUnload()
{
	return NULL;
}

LIBAPI void ControllerBase::Load(ObjectClass* data)
{
	OnLoad(data);
}

LIBAPI ObjectClass* ControllerBase::Unload()
{
	return OnUnload();
}

LIBAPI void ControllerBase::OnReload(ObjectClass* data)
{
}

LIBAPI void ControllerBase::Reload(ObjectClass* data)
{
	OnReload(data);
}

LIBAPI ControllerBase::~ControllerBase()
{
}


void SRIN::Framework::ControllerBase::OnLeaving() {
}

void SRIN::Framework::ControllerBase::Leave() {
	this->OnLeaving();
}
