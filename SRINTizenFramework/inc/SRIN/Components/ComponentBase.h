/*
 * ComponentBase.h
 *
 *  Created on: Feb 19, 2016
 *      Author: gilang
 */

#ifndef COMPONENTBASE_H_
#define COMPONENTBASE_H_

#include "SRIN/Core.h"
#include "SRIN/ELMInterface.h"

#include <string>

namespace SRIN { namespace Components {

	class LIBAPI ComponentBase : virtual public EventClass {
	private:
		bool created;
		bool enabled;
		bool visible;
		std::string name;
	protected:
		Evas_Object* componentRoot;
		double weightX, weightY, alignX, alignY;
		virtual LIBAPI Evas_Object* CreateComponent(Evas_Object* root) = 0;

		virtual LIBAPI void SetEnabled(const bool& enabled);
		virtual LIBAPI bool GetEnabled();

		virtual LIBAPI void SetVisible(const bool& visible);
		virtual LIBAPI bool GetVisible();

		virtual LIBAPI void SetName(const std::string& name);
		virtual LIBAPI std::string GetName();
	public:
		ComponentBase();
		inline bool IsCreated() { return created; }
		Evas_Object* LIBAPI Create(Evas_Object* root);

		Property<ComponentBase, bool, &ComponentBase::GetEnabled, &ComponentBase::SetEnabled> Enabled;
		Property<ComponentBase, bool, &ComponentBase::GetVisible, &ComponentBase::SetVisible> Visible;
		Property<ComponentBase, std::string, &ComponentBase::GetName, &ComponentBase::SetName> Name;

		virtual LIBAPI ~ComponentBase();
		operator LIBAPI Evas_Object*() const;
	};

}}

#endif /* COMPONENTBASE_H_ */
