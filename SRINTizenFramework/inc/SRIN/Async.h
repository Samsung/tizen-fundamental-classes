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

namespace SRIN {

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

/**
 * Handle for every asynchronous task that is performed using async keyword. The handle of this object is returned when
 * calling async. The user needs to store the handle to this object to refer to the task on later operation.
 */

template<class R>
class AsyncTask;

class AsyncTaskObj;

void dwait(AsyncTask<void>* task, Event<AsyncTask<void>*>& eventTarget);

template<class R>
void dwait(AsyncTask<R>* task, Event<AsyncTask<R>*, R>& eventTarget);

template<class R>
AsyncTask<R>* AsyncCall(std::function<R(void)> func);

template<class V>
AsyncTaskObj* CreateAsyncTask(std::function<V(void)> func);

template<>
AsyncTaskObj* CreateAsyncTask(std::function<void*(void)> func);

template<>
AsyncTaskObj* CreateAsyncTask(std::function<void(void)> func);

void* AwaitImpl(AsyncTaskObj* task);
void DwaitImplVal(AsyncTaskObj* task, std::function<void(void*)> dispatcher);
void AbortImpl(AsyncTaskObj* task);

struct AsyncBuilder
{
public:
	template<class Lambda>
	AsyncTask<typename function_traits<Lambda>::result_type>* operator+(Lambda lambda)
	{
		typedef function_traits<Lambda> trait;
		return AsyncCall(std::function<typename trait::result_type()>(lambda));
	}
};

template<class R>
struct DispatchAwaitBuilder
{
	AsyncTask<R>* task;
	Event<AsyncTask<R>*, R>& eventTarget;
};

struct AwaitBuilder
{
public:
	template<class R>
	R operator&(AsyncTask<R>* task)
	{
		std::unique_ptr<R> ret(reinterpret_cast<R*>(AwaitImpl(reinterpret_cast<AsyncTaskObj*>(task))));
		return *ret;
	}

	template<class R>
	void operator&(DispatchAwaitBuilder<R> dwaitBuild)
	{
		dwait(dwaitBuild.task, dwaitBuild.eventTarget);
	}
};

struct AbortBuilder
{
public:
	template<class R>
	void operator&(AsyncTask<R>* task)
	{
		AbortImpl(reinterpret_cast<AsyncTaskObj*>(task));
	}
};



template<class R>
DispatchAwaitBuilder<R> operator>>(AsyncTask<R>* task, Event<AsyncTask<R>*, R>& eventTarget)
{
	return { task, eventTarget };
}

bool IsAborting();

#define async AsyncBuilder() +
#define await AwaitBuilder() &
#define abort_return if(IsAborting()) return
#define abort_async AbortBuilder() &

template<class R>
AsyncTask<R>* AsyncCall(std::function<R(void)> func)
{
	return reinterpret_cast<AsyncTask<R>*>(CreateAsyncTask(std::function<void*(void)>([func] () -> void* {
		R ret = func();
		void* retPtr = malloc(sizeof(R));
		memcpy(retPtr, &ret, sizeof(R));
		return retPtr;
	})));
}

template<>
AsyncTask<void>* AsyncCall<void>(std::function<void(void)> func);

template<class R>
void dwait(AsyncTask<R>* task, Event<AsyncTask<R>*, R>& eventTarget)
{
	DwaitImplVal(reinterpret_cast<AsyncTaskObj*>(task), [task, eventTarget] (void* r) {
		std::unique_ptr<R> ret(reinterpret_cast<R*>(r));
		eventTarget(task, *ret);
	});
}


}


#endif /* ASYNCTASK_H_ */
