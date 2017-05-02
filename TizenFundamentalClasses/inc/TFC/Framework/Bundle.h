/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Framework/Bundle.h
 *
 * Simplification on passing data between classes instead of using tuple,
 * which does not provide runtime type checking.
 *
 * Created on: 	Sep 16, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
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
