/*
 * ControllerManager.cpp
 *
 *  Created on: Feb 15, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "TFC/Framework/Application.h"
#include "TFC/Framework/ControllerManager.h"
#include <memory>
#include <sstream>
using namespace TFC::Framework;

void ControllerManager_FreeFactory(void* data)
{
	ControllerFactory* factory = static_cast<ControllerFactory*>(data);
	delete factory;
}

TFC::Framework::ControllerManager::ControllerManager() :
	CurrentController(this)
{

}

LIBAPI void ControllerManager::RegisterControllerFactory(ControllerFactory* ptr)
{
	std::unique_ptr<ControllerFactory> controller(ptr);
	auto iter = this->controllerTable.find(controller->controllerName);
	if(iter == this->controllerTable.end())
	{
		this->controllerTable[controller->controllerName] = controller->factoryMethod;
	}
	else
		throw TFCException("Controller already registered");
}

TFC::Framework::ControllerManager::~ControllerManager()
{
}

LIBAPI
StackingControllerManager::StackingControllerManager()
{

}



LIBAPI
void ControllerManager::NavigateTo(char const* controllerName, ObjectClass* data, NavigationFlag mode)
{
	if(this->pendingNavigation)
		throw TFCException("There is pending navigation");

	this->pendingNavigation = true;
	InvokeLater(&ControllerManager::PerformNavigationInternal, controllerName, data, mode);
}

LIBAPI
void ControllerManager::PerformNavigationInternal(char const* controllerName, ObjectClass* data, NavigationFlag mode)
{
	PerformNavigation(controllerName, data, mode);
	this->pendingNavigation = false;
}

void StackingControllerManager::PushController(ControllerBase* controller)
{
	this->controllerStack.emplace_back(controller);
}

bool StackingControllerManager::PopController()
{
	this->controllerStack.pop_back();

	if (!this->controllerStack.empty())
		return true;
	else
		return false;
}



LIBAPI ControllerFactory::ControllerFactory(char const* controllerName, ControllerFactoryMethod factory) :
	controllerName(controllerName), factoryMethod(factory)
{
	attachedData = nullptr;
}

LIBAPI ControllerBase& TFC::Framework::StackingControllerManager::GetCurrentController() const
{
	return *(this->controllerStack.back());
}

LIBAPI
void TFC::Framework::StackingControllerManager::DoNavigateBackward()
{
	// Navigate back
	ObjectClass* returnedData = this->CurrentController->Unload();
	PopView();
	bool popResult = PopController();

	if (popResult)
	{

		ControllerBase& current = this->CurrentController;
		current.Reload(returnedData);
		eventNavigationProcessed(this, &current);
	}

	// If pop result is false, it is the end of the controller
	// It should end the application
	// TODO: Add this as event instead
	if(!popResult)
		ui_app_exit();
}

Evas_Object* StackingControllerManager::CreateViewContainer(Evas_Object* parent)
{
	auto naviframe = elm_naviframe_add(parent);
	elm_naviframe_prev_btn_auto_pushed_set(naviframe, EINA_TRUE);
	evas_object_size_hint_weight_set(naviframe, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(naviframe, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(naviframe);
	this->viewContainer = naviframe;
	return naviframe;
}

void StackingControllerManager::PushView(ViewBase* view)
{
	Evas_Object* viewComponent = view->Create(this->viewContainer);

	//show to window
	if (viewComponent != NULL)
	{
		auto naviframeContent = dynamic_cast<INaviframeContent*>(view);

		char const* naviframeStyle = nullptr;

		if (naviframeContent)
			naviframeStyle = naviframeContent->GetContentStyle();

		auto naviframeItem = elm_naviframe_item_push(this->viewContainer, view->Title->c_str(), NULL, NULL,
			viewComponent, naviframeStyle);

		auto backButton = elm_object_item_part_content_get(naviframeItem, "elm.swallow.prev_btn");
		auto style = elm_object_style_get(backButton);

		// Title button handling

		if (naviframeContent)
		{
			char const* buttonPart = "title_left_btn";
			auto left = naviframeContent->GetTitleLeftButton(&buttonPart);
			if (left)
			{
				auto oldObj = elm_object_item_part_content_unset(naviframeItem, buttonPart);
				evas_object_hide(oldObj);
				elm_object_item_part_content_set(naviframeItem, buttonPart, left);
				evas_object_show(left);
			}

			buttonPart = "title_right_btn";
			auto right = naviframeContent->GetTitleRightButton(&buttonPart);
			if (right)
			{
				auto oldObj = elm_object_item_part_content_unset(naviframeItem, buttonPart);
				evas_object_hide(oldObj);
				elm_object_item_part_content_set(naviframeItem, buttonPart, right);
				evas_object_show(right);
			}

			naviframeContent->RaiseAfterNaviframePush(naviframeItem);
		}

		evas_object_smart_callback_add(backButton, "clicked", [] (void* a, Evas_Object* b, void* c)
		{	static_cast<UIApplicationBase*>(a)->BackButtonPressed();}, this);

		evas_object_show(viewComponent);
	}
}

void StackingControllerManager::PopView()
{
	elm_naviframe_item_pop(this->viewContainer);
}

LIBAPI
void TFC::Framework::StackingControllerManager::PerformNavigation(char const* controllerName, ObjectClass* data, TFC::Framework::NavigationFlag mode)
{
	if(mode & NavigationFlag::Back)
	{
		return DoNavigateBackward();
	}
	else if(mode & NavigationFlag::ClearHistory)
	{
		// Remove all controller from history
		while(!this->controllerStack.empty())
		{
			if(not(mode & NavigationFlag::NoCallUnload))
			{
				this->CurrentController->Unload();
			}

			PopView();
			PopController();
		}

		goto PerformNavigation_Default;
	}
	else if(mode & NavigationFlag::NoTrail)
	{
		// Discard current controller from history
		if(not(mode & NavigationFlag::NoCallUnload))
		{
			this->CurrentController->Unload();
		}

		PopView();
		PopController();
		goto PerformNavigation_Default;
	}
	else if(mode & NavigationFlag::Default)
	{
		PerformNavigation_Default:

		// Instantiate controller
		ControllerBase* newInstance = this->Instantiate(controllerName);

		// Perform OnLeave on previous controller
		if(!this->controllerStack.empty())
			this->CurrentController->Leave();

		PushController(newInstance);
		PushView(newInstance->View);

		// Instantiated State, move to Running state
		newInstance->Load(data);
		eventNavigationProcessed(this, newInstance);

		return;
	}

	throw TFCException("The navigation mode is invalid or not implemented");
}

ControllerBase* ControllerFactory::Instantiate(ControllerManager* mgr)
{
	return this->factoryMethod(mgr);
}

ControllerBase* ControllerManager::Instantiate(const char* controllerName)
{
	try
	{
		auto method = this->controllerTable.at(controllerName);
		return method(this);
	}
	catch(std::out_of_range& ex)
	{
		std::stringstream errBuilder;
		errBuilder << "Unknown controller identifier: " << controllerName;
		throw TFCException(errBuilder.str());
	}
}


