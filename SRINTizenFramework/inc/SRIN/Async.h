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
AsyncTask<R>* AsyncCall(std::function<R(void)> func, std::function<void(void*, void*)> dispatcher);

template<class V>
AsyncTaskObj* CreateAsyncTask(std::function<V(void)> func, std::function<void(void*, void*)> dispatcher);

template<>
AsyncTaskObj* CreateAsyncTask(std::function<void*(void)> func, std::function<void(void*, void*)> dispatcher);

template<>
AsyncTaskObj* CreateAsyncTask(std::function<void(void)> func, std::function<void(void*, void*)> dispatcher);

void* AwaitImpl(AsyncTaskObj* task);
void DwaitImplVal(AsyncTaskObj* task, std::function<void(void*, void*)> dispatcher);
void AbortImpl(AsyncTaskObj* task);

template<class R>
struct DispatchAwaitBuilder
{
	AsyncTask<R>* task;
	Event<AsyncTask<R>*, R>& eventTarget;
};

template<class ReturnType>
struct DispatchAsyncBuilder
{
	typedef ReturnType R;
	typedef AsyncTask<R> AsyncTaskType;
	typedef Event<AsyncTaskType*, R> EventType;
	std::function<R(void)> lambda;
	EventType& eventTarget;
};

template<>
struct DispatchAsyncBuilder<void>
{
	typedef void R;
	typedef AsyncTask<void> AsyncTaskType;
	typedef Event<AsyncTaskType*> EventType;
	std::function<void(void)> lambda;
	EventType& eventTarget;
};

// Cancelling function trait for type DispatchAsyncBuilder so the overload can call correct function
template <>
template<class Lambda>
struct function_traits<DispatchAsyncBuilder<Lambda>>
{};

template <>
template<class R>
struct function_traits<AsyncTask<R>*>
{};

template<>
template<class R>
struct DispatchAsyncBuilder<AsyncTask<R>*>
{};

template<class ReturnValue>
struct Async
{
	typedef AsyncTask<ReturnValue> Task;
	typedef Event<Task*, ReturnValue> Event;
};

template<>
struct Async<void>
{
	typedef AsyncTask<void> Task;
	typedef Event<Task*> Event;
};

std::function<void(void*, void*)> GetDispatcher(Event<AsyncTask<void>*>& eventTarget);

template<class R>
std::function<void(void*, void*)> GetDispatcher(Event<AsyncTask<R>*, R>& eventTarget)
{
	return [eventTarget] (void* t, void* r) {
		std::unique_ptr<R> ret(reinterpret_cast<R*>(r));
		eventTarget(reinterpret_cast<AsyncTask<R>*>(t), *ret);
	};
}

struct AsyncBuilder
{
	template<class Lambda>
	AsyncTask<typename function_traits<Lambda>::result_type>* operator&(Lambda lambda)
	{
		std::function<void(void*, void*)> dispatcher;
		typedef function_traits<Lambda> trait;
		return AsyncCall(std::function<typename trait::result_type()>(lambda), dispatcher);
	}

	template<class T>
	AsyncTask<typename DispatchAsyncBuilder<T>::R>* operator&(DispatchAsyncBuilder<T> builder)
	{
		typedef typename DispatchAsyncBuilder<T>::R R;
		auto eventTarget = builder.eventTarget;
		std::function<void(void*, void*)> dispatcher = GetDispatcher(builder.eventTarget);
		return AsyncCall(builder.lambda, dispatcher);
	}
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

template<class Lambda>
DispatchAsyncBuilder<typename function_traits<Lambda>::result_type> operator>>(Lambda lambda, typename DispatchAsyncBuilder<typename function_traits<Lambda>::result_type>::EventType& eventTarget)
{
	return { lambda, eventTarget };
}

bool IsAborting();

#define s_async AsyncBuilder() &
#define s_await AwaitBuilder() &
#define s_abort_return if(IsAborting()) return
#define s_abort_async AbortBuilder() &

template<class R>
AsyncTask<R>* AsyncCall(std::function<R(void)> func, std::function<void(void*, void*)> dispatcher)
{
	return reinterpret_cast<AsyncTask<R>*>(CreateAsyncTask(std::function<void*(void)>([func] () -> void* {
		R ret(func());
		void* retPtr = malloc(sizeof(R));
		memcpy(retPtr, &ret, sizeof(R));
		return retPtr;
	}), dispatcher));
}

template<>
AsyncTask<void>* AsyncCall<void>(std::function<void(void)> func, std::function<void(void*, void*)> dispatcher);

template<class R>
void dwait(AsyncTask<R>* task, Event<AsyncTask<R>*, R>& eventTarget)
{
	DwaitImplVal(reinterpret_cast<AsyncTaskObj*>(task), [eventTarget] (void* t, void* r) {
		std::unique_ptr<R> ret(reinterpret_cast<R*>(r));
		dlog_print(DLOG_DEBUG, LOG_TAG, "TypeID: %s", typeid(eventTarget).name());
		eventTarget(reinterpret_cast<AsyncTask<R>*>(t), *ret);
	});
}


}


#endif /* ASYNCTASK_H_ */
