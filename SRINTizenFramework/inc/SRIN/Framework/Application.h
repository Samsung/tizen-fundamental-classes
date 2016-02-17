/*
 * Application.h
 *
 *  Created on: Feb 12, 2016
 *      Author: gilang
 */

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <app.h>

extern "C"
{
#include <eina_hash.h>
}

#include <system_settings.h>
#include <efl_extension.h>

#include <string>
#include <vector>

#include "SRIN/Core.h"
#include "SRIN/ELMInterface.h"

#define RegisterController(CONTROLLER_NAME, CONTROLLER_TYPE) RegisterControllerFactory(new SRIN::Framework::ControllerFactory(CONTROLLER_NAME, [] (SRIN::Framework::ControllerManager* m) -> SRIN::Framework::ControllerBase* { return new CONTROLLER_TYPE (m); }))



namespace SRIN { namespace Framework {
	class ViewBase;

	class ControllerManager;
	class ApplicationBase;

	typedef bool (EventClass::*BackButtonCallback)();

	/**
	 * Interface that represents an attachable object. A class that implements this
	 * interface should be able to receive a ViewBase and attach its content internally
	 */
	class LIBAPI IAttachable {
	public:

		/**
		 * Method to be called to attach a view inside this attachable object
		 *
		 * @param view A view to be attached
		 */
		virtual LIBAPI void Attach(ViewBase* view) = 0;

		/**
		 * Method to be called to detach the currently attached view from the object
		 */
		virtual LIBAPI void Detach() = 0;

		/**
		 * Virtual destructor for the interface
		 */
		virtual LIBAPI ~IAttachable();
	};

	/**
	 * Application base class that encapsulates a simple Tizen application. It contains a simple
	 * implementation of a Tizen application, and several overridable methods that can be overriden
	 * for specific Tizen system callback implementation.
	 *
	 * ApplicationBase class encapsulates window frame with Naviframe object as its root component.
	 * Any view class inheriting ViewBase can be attached to this application and will be displayed
	 * on to screen.
	 */
	class LIBAPI ApplicationBase : public IAttachable {
	private:

		bool haveEventBackPressed;
		EventClass* backButtonInstance;

		BackButtonCallback backButtonCallback;
	public:
		/**
		 * Root win object
		 */
		Evas_Object* win;

		/**
		 * Root conform object
		 */
		Evas_Object* conform;

		/**
		 * Root frame object. Should never be modified in any condition
		 */
		Evas_Object* rootFrame;

		const CString packageName;

		/**
		 * Constructor of ApplicationBase class
		 */
		LIBAPI ApplicationBase(CString packageName);

		/**
		 * Method that will be called after the application creation process is finished. At this
		 * point, the application is ready to receive Attach action from any component.
		 */
		virtual LIBAPI void OnApplicationCreated();

		/**
		 * Method that will be called by Tizen system for any Control event received. Override this
		 * method to react to the event.
		 */
		virtual LIBAPI void ApplicationControl(app_control_h app_control);

		/**
		 * Method that will be called by Tizen system for any Pause event received. Override this
		 * method to react to the event.
		 */
		virtual LIBAPI void ApplicationPause();

		/**
		 * Method that will be called by Tizen system for any Resume event received. Override this
		 * method to react to the event.
		 */
		virtual LIBAPI void ApplicationResume();

		/**
		 * Method that will be called by Tizen system for any Terminate event received. Override
		 * this method to react to the event.
		 */
		virtual LIBAPI void ApplicationTerminate();

		/**
		 * Method that will be called by Tizen system for any Language Changed event received.
		 * Override this method to react to the event.
		 */
		virtual LIBAPI void LanguageChanged(app_event_info_h event_info, const char* locale);

		/**
		 * Method that will be called by Tizen system for any Orientation Changed event received.
		 * Override this method to react to the event.
		 */
		virtual LIBAPI void OrientationChanged(app_event_info_h event_info);

		/**
		 * Method that will be called by Tizen system for any Region Changed event received.
		 * Override this method to react to the event.
		 */
		virtual LIBAPI void RegionChanged(app_event_info_h event_info);

		/**
		 * Method that will be called by Tizen system for any Low Battery event received. Override
		 * this method to react to the event.
		 */
		virtual LIBAPI void LowBattery(app_event_info_h event_info);

		/**
		 * Method that will be called by Tizen system for any Low Memory event received. Override
		 * this method to react to the event.
		 */
		virtual LIBAPI void LowMemory(app_event_info_h event_info);

		/**
		 * Method that will be called when the user called back button. Override this method to
		 * handle specific behavior when user clicks the back button.
		 *
		 * @return true if the application should be closed after clicking back
		 * 		   false to cancel closing the application
		 */
		virtual LIBAPI bool OnBackButtonPressed();

		LIBAPI void EnableBackButtonCallback(bool enable);
		LIBAPI bool AcquireExclusiveBackButtonPressed(EventClass* instance, BackButtonCallback callback);

		LIBAPI bool ReleaseExclusiveBackButtonPressed(EventClass* instance, BackButtonCallback callback);

		/**
		 * Method to be called to attach a view inside this attachable object
		 *
		 * @param view A view to be attached
		 */
		LIBAPI void Attach(ViewBase* view);

		/**
		 * Method to be called to detach the currently attached view from the object
		 */
		LIBAPI void Detach();

		/**
		 * Method to initiate exit the application
		 */
		LIBAPI void Exit();

		/**
		 * Method that will be called by Tizen system when the user clicks the back button
		 */
		LIBAPI void BackButtonPressed();

		/**
		 * Method that will be called by Tizen system to create the application.
		 */
		LIBAPI bool ApplicationCreate();

		/**
		 * Main method to start the application loop using an ApplicationBase instance
		 */
		static LIBAPI int Main(ApplicationBase* app, int argc, char *argv[]);

		/**
		 * Destructor of ApplicationBase
		 */
		virtual LIBAPI ~ApplicationBase();
	};

	/**
	 * Controller base class for encapsulation of business logic from view logic. Each controller
	 * will have one default view which will display data and receive action on behalf of this
	 * controller.
	 *
	 * Create a class which inherits this class to create a controller for the application. A
	 * controller will be managed by ControllerManager to support navigation between controllers,
	 * and control passing.
	 */
	class LIBAPI ControllerBase : virtual public EventClass {
	protected:
		/**
		 * Reference to ControllerManager
		 */
		ControllerManager* const Manager;

		/**
		 * Method that will be called during the loading of this controller. Override this method
		 * to implement specific action for controller loading.
		 *
		 * @param data Data received from other controller which calls this controller
		 *
		 * @note The data parameter must be freed by this controller as the owner of the object is
		 * 		 passed onto this controller.
		 */
		virtual LIBAPI void OnLoad(void* data);

		/**
		 * Method that will be called during the unloading of this controller. This method will be
		 * called when it is navigating back from this controller to its caller-controller.
		 *
		 * @return Data returned to be passed back to the calling controller
		 *
		 * @note The data to be returned must be allocated in the heap, and the responsible party
		 * 		 to clean up the data is the receiver controller.
		 */
		virtual LIBAPI void* OnUnload();

		/**
		 * Method that will be called during the reloading of this controller after returning from
		 * other controller. Override this method to implement specific action for controller
		 * reloading.
		 *
		 * @param data Data retrieved from the previously called controller (i.e. returned by
		 * 			   OnUnload method of called controller).
		 *
		 * @note The data parameter must be freed by this controller as the owner of the object is
		 * 		 passed onto this controller.
		 */
		virtual LIBAPI void OnReload(void* data);
	public:
		/**
		 * The standard constructor for ControllerBase class.
		 *
		 * @param manager ControllerManager that is automatically populated during the instantiation
		 * 				  of the controller
		 * @param view ViewBase instance that will be the main view of this controller
		 * @param controllerName The unique name of this controller
		 *
		 * @note In order to follow the standardized factory pattern for controller classes, the
		 * 		 implementing class _must follow standardized constructor signature_ as follow:
		 * 		 `CustomController(Framework::ControllerManager* manager)`. If the implementing
		 * 		 class does not create the standard constructor, the factory mechanism to register
		 * 		 the controller to the ControllerManager will not work.
		 *
		 * @see {RegisterController}
		 */
		LIBAPI ControllerBase(ControllerManager* manager, ViewBase* view, CString controllerName);

		/**
		 * The reference to the view instance for this controller
		 */
		ViewBase* const View;

		/**
		 * The name of this controller
		 */
		CString const ControllerName;

		/**
		 * The title of this controller
		 */
		CString Title;

		/**
		 * Method that will be called by ControllerManager to load the controller
		 *
		 * @param data Parameter to pass to this controller
		 */
		LIBAPI void Load(void* data);

		/**
		 * Method that will be called by ControllerManager to unload the controller
		 *
		 * @return The returned data from this controller
		 */
		LIBAPI void* Unload();

		/**
		 * Method that will be called by ControllerManager to reload the controller
		 *
		 * @param data Parameter to pass to this controller
		 */
		LIBAPI void Reload(void* data);

		/**
		 * Destructor of ControllerBase instance
		 */
		virtual LIBAPI ~ControllerBase();
	};

	/**
	 * Function pointer to instantiate the specific controller. Should be used along the macro
	 * RegisterController
	 */
	typedef ControllerBase* (*ControllerFactoryMethod) (ControllerManager*);

	/**
	 * Class that encapsulate dynamic controller instatiation which is required by ControllerManager
	 */
	class LIBAPI ControllerFactory {
	public:
		/**
		 * Constructor of ControllerFactory
		 *
		 * @param controllerName Name of the controller
		 * @param factory Factory method which will instantiate the controller
		 */
		LIBAPI ControllerFactory(CString controllerName, ControllerFactoryMethod factory);

		CString const controllerName;
		ControllerFactoryMethod const factoryMethod;
	};

	/**
	 * Class that encapsulate controller stack within ControllerManager
	 */
	class LIBAPI ControllerChain {
	public:
		ControllerBase* instance;
		ControllerChain* next;
	};

	/**
	 * Class that manages Controller and provides navigation mechanism between loaded controllers
	 */
	class LIBAPI ControllerManager {
	private:
		Eina_Hash* controllerTable;
		ControllerChain* chain;
		IAttachable* const app;

		void PushController(ControllerBase* controller);
		bool PopController();
	public:
		/**
		 * Constructor of ControllerManager
		 *
		 * @param app IAttachable which this controller manager will attach the underlying view
		 */
		LIBAPI ControllerManager(IAttachable* app);

		/**
		 * Method to register ControllerFactory to this manager so this manager can recognize
		 * the controller and instantiate it when needed
		 */
		LIBAPI void RegisterControllerFactory(ControllerFactory* controller);
		LIBAPI void NavigateTo(const char* controllerName, void* data);
		LIBAPI void NavigateTo(const char* controllerName, void* data, bool noTrail);
		LIBAPI bool NavigateBack();
	};

	class LIBAPI MVCApplicationBase : public ApplicationBase, public ControllerManager {
	private:
		CString mainController;
	public:
		LIBAPI MVCApplicationBase(CString appPackage, CString mainController);
		virtual LIBAPI bool OnBackButtonPressed() final;
		virtual LIBAPI void OnApplicationCreated() final;
		virtual LIBAPI ~MVCApplicationBase() { }
	};


	class LIBAPI ComponentBase : virtual public EventClass {
	private:
		bool created;
		bool enabled;
		bool visible;
		CString name;
	protected:
		Evas_Object* componentRoot;
		double weightX, weightY, alignX, alignY;
		virtual LIBAPI Evas_Object* CreateComponent(Evas_Object* root) = 0;

		virtual LIBAPI void SetEnabled(const bool& enabled);
		virtual LIBAPI bool GetEnabled();

		virtual LIBAPI void SetVisible(const bool& visible);
		virtual LIBAPI bool GetVisible();

		virtual LIBAPI void SetName(const CString& name);
		virtual LIBAPI CString GetName();
	public:
		ComponentBase();
		inline bool IsCreated() { return created; }
		Evas_Object* LIBAPI Create(Evas_Object* root);

		Property<ComponentBase, bool, &ComponentBase::GetEnabled, &ComponentBase::SetEnabled> Enabled;
		Property<ComponentBase, bool, &ComponentBase::GetVisible, &ComponentBase::SetVisible> Visible;
		Property<ComponentBase, CString, &ComponentBase::GetName, &ComponentBase::SetName> Name;

		virtual LIBAPI ~ComponentBase();
		operator LIBAPI Evas_Object*() const;
	};

	/**
	 * Abstract base class for encapsulation of View definition and logic. Each view class represent
	 * a user interface definition which will display data and interact directly with user.
	 *
	 * Create a class which inherits from this base class to create a user interface class. Instance
	 * of this class can be attached into an IAttachable instance to display it to designated window
	 * or placeholder.
	 */
	class LIBAPI ViewBase : virtual public EventClass {
	private:
		Evas_Object* viewRoot;
	protected:
		virtual LIBAPI Evas_Object* CreateView(Evas_Object* root) = 0;
	public:
		LIBAPI ViewBase(CString viewName);
		CString const viewName;

		virtual LIBAPI Evas_Object* Create(Evas_Object* root);
		virtual LIBAPI CString GetStyle();

		LIBAPI bool IsCreated();
		LIBAPI Evas_Object* GetViewRoot();

		virtual LIBAPI ~ViewBase();
	};

	class LIBAPI ITitleButton {
	public:
		virtual LIBAPI Evas_Object* GetTitleLeftButton() = 0;
		virtual LIBAPI Evas_Object* GetTitleRightButton() = 0;
		virtual LIBAPI ~ITitleButton();
	};
}
}


#endif
