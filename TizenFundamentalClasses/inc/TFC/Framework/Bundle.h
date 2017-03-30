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
#include <tuple>

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

			std::unique_ptr<Bundle<T>> bundlePtr(bundle);
			return std::move(bundlePtr->value);
		}
	};

	template<typename... TArgs>
	class Tuple : public ObjectClass
	{
	private:
		std::tuple<TArgs...> values;
	public:
		template<typename... T>
		Tuple(T&&... args) :
			values(std::forward<T>(args)...)
		{
		}

		template<std::size_t i>
		auto Get()
		{
			return std::get<i>(values);
		}

		template<typename T>
		auto Get()
		{
			return std::get<T>(values);
		}

		static Tuple<TArgs...>* TryCast(ObjectClass* data)
		{
			auto tuple = dynamic_cast<Tuple<TArgs...>*>(data);
			if (tuple == nullptr)
				throw std::runtime_error("ObjectClass is not a correct tuple instance");

			return tuple;
		}
	};

	template<typename... TArgs>
	auto MakeTuple(TArgs&&... args)
	{
		return new Tuple<typename std::decay<TArgs>::type...>(std::forward<TArgs>(args)...);
	}
}
}



#endif /* BUNDLE_H_ */
