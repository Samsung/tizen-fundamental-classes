/*
 * Bundle.h
 *
 *  Created on: Sep 16, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef BUNDLE_H_
#define BUNDLE_H_

#include "TFC/Core.h"
#include <stdexcept>
#include <memory>

namespace TFC {
namespace Framework {

	template<class T>
	class Bundle : public ObjectClass
	{
	private:
		T value;
		Bundle(T& value)
		{
			this->value = value;
		}
	public:


		static Bundle<T>* Pack(T& value)
		{
			return new Bundle<T>(value);
		}

		static T Unpack(ObjectClass* data)
		{
			auto bundle = dynamic_cast<Bundle<T>*>(data);
			if(bundle == nullptr)
				throw std::runtime_error("ObjectClass is not a correct bundle instance");

			std::shared_ptr<Bundle<T>> bundlePtr(bundle);
			return bundlePtr->value;
		}
	};

}
}



#endif /* BUNDLE_H_ */
