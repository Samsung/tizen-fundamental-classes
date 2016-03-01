/*
 * AsyncTask.h
 *
 * Library to handle asynchronous processing.
 *
 *  Created on: Mar 1, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef ASYNCTASK_H_
#define ASYNCTASK_H_

#include <functional>
#include <memory>
#include "SRIN/Core.h"


/**
 * Handle for every asynchronous task that is performed using async keyword. The handle of this object is returned when
 * calling async. The user needs to store the handle to this object to refer to the task on later operation.
 */
class AsyncTask;


template<class Lambda>
AsyncTask* async(Lambda lambda);

template<class R>
R await(AsyncTask* task);

template<class R>
void dwait(AsyncTask* task, Event<R>& eventTarget);


/*
 * http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda
 */
template <typename T>
struct function_traits
    : public function_traits<decltype(&T::operator())>
{};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
{
    enum { arity = sizeof...(Args) };

    typedef ReturnType result_type;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template<class R>
AsyncTask* AsyncCall(std::function<R(void)> func);

template<class V>
AsyncTask* CreateAsyncTask(std::function<V(void)> func);

template<>
AsyncTask* CreateAsyncTask(std::function<void*(void)> func);

template<>
AsyncTask* CreateAsyncTask(std::function<void(void)> func);

void* AwaitImpl(AsyncTask* task);


template<>
void await(AsyncTask* task);

template<class R>
AsyncTask* AsyncCall(std::function<R(void)> func)
{
	return CreateAsyncTask(std::function<void*(void)>([func] () -> void* {
		R ret = func();
		void* retPtr = malloc(sizeof(R));
		memcpy(retPtr, &ret, sizeof(R));
		return retPtr;
	}));
}

template<>
AsyncTask* AsyncCall<void>(std::function<void(void)> func)
{
	return CreateAsyncTask(std::function<void(void)>([func] () -> void {
		func();
	}));
}

template<class R>
R await(AsyncTask* task)
{
	std::unique_ptr<R> ret(reinterpret_cast<R*>(AwaitImpl(task)));
	return *ret;
}

template<class Lambda>
AsyncTask* async(Lambda lambda)
{
	typedef function_traits<Lambda> trait;
	return AsyncCall(std::function<typename trait::result_type()>(lambda));
}

template<class R>
void dwait(AsyncTask* task, Event<R>& eventTarget)
{
}

#endif /* ASYNCTASK_H_ */
