/*
 * Adapter.h
 *
 *  Created on: Mar 8, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_ADAPTER_H_
#define SRIN_ADAPTER_H_

#include "SRIN/Components/ComponentBase.h"

#include <vector>
#include <unordered_map>
#include "SRIN/Core.h"
#include "SRIN/ELMInterface.h"

namespace SRIN {
namespace Components {

	class

	class Adapter
	{
	private:
		struct AdapterItemEntry
		{
			void* item;
			Elm_Object_Item* elmObject;
		};

		std::vector<AdapterItemEntry> itemEntry;
	public:

	};

}
}

#endif /* ADAPTER_H_ */
