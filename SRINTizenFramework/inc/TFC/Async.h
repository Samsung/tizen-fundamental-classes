/*
 * Async.h
 *
 *  Created on: Nov 2, 2016
 *      Author: Gilang M. Hamidy
 */

#pragma once

#ifndef TFC_CORE_ASYNC_NEW_H_
#define TFC_CORE_ASYNC_NEW_H_

#include <functional>

#include "TFC/Core.h"
#include "TFC/Core/Introspect.h"

namespace TFC {

template<typename TReturnValue>
struct Async;


namespace Core {
namespace Async {

template<typename TReturnValue>
struct AsyncTask;

template<typename TReturnValue, typename TEventType = void>
struct AsyncOperand
{
	typedef TReturnValue											  ReturnType;
	typedef AsyncTask<TReturnValue>									  AsyncTaskType;
	typedef SharedEventObject<AsyncTask<TReturnValue>*, TReturnValue> EventType;
	std::function<ReturnType(void)> asyncFunc;

	AsyncOperand(std::function<ReturnType(void)>&& asyncFunc) :
		asyncFunc(asyncFunc)
	{ }

	template<typename TLambda,
			 typename TIntrospect = Introspect::CallableObject<TLambda>,
			 typename 			  = typename std::enable_if<TIntrospect::Arity == 0>::type>
	AsyncOperand(TLambda aLambda) :
		asyncFunc(aLambda)
	{ }
};



template<>
struct AsyncOperand<void, void>
{
	typedef void											ReturnType;
	typedef AsyncTask<void>									AsyncTaskType;
	typedef SharedEventObject<AsyncTask<void>*, void*> 		EventType;

	std::function<void(void)> asyncFunc;

	AsyncOperand(std::function<void(void)>&& func) :
		asyncFunc(func)
	{ }

	template<typename TLambda,
				 typename TIntrospect = Introspect::CallableObject<TLambda>,
				 typename 			  = typename std::enable_if<TIntrospect::Arity == 0>::type>
	AsyncOperand(TLambda aLambda) :
		asyncFunc(aLambda)
	{ }
};

template<>
struct AsyncOperand<void, SharedEventObject<AsyncTask<void>*, void*>> :
		AsyncOperand<void, void>
{

	AsyncOperand<void, void>::EventType&	eventRef;

	AsyncOperand(std::function<void(void)>&& func, AsyncOperand<void, void>::EventType& eventRef) :
		AsyncOperand<void, void>(std::move(func)),
		eventRef(eventRef)
	{ }
};

template<typename TReturnValue>
struct AsyncOperand<TReturnValue, SharedEventObject<AsyncTask<TReturnValue>*, TReturnValue>> :
		AsyncOperand<TReturnValue, void>
{

	typename AsyncOperand<TReturnValue, void>::EventType&	eventRef;

	AsyncOperand(std::function<TReturnValue(void)>&& func, typename AsyncOperand<TReturnValue, void>::EventType& eventRef) :
		AsyncOperand<TReturnValue, void>(std::move(func)),
		eventRef(eventRef)
	{ }
};

template<typename TLambda,
		 typename TIntrospect = Introspect::CallableObject<TLambda>,
		 typename TEnable 	  = typename std::enable_if<TIntrospect::Arity == 1>::type>
struct AsyncCompleteOperand
{
	typedef typename Introspect::CallableObject<TLambda>::template Args<0> ArgumentType;
	static constexpr bool Valid = true;

	std::function<void(ArgumentType)> CompleteLambda;

	explicit AsyncCompleteOperand(std::function<void(ArgumentType)>&& CompleteLambda) :
			CompleteLambda(CompleteLambda)
	{

	}
};

template<typename TReturnValue, typename TLambdaComplete>
struct AsyncOperand<TReturnValue, AsyncCompleteOperand<TLambdaComplete>> :
		AsyncOperand<TReturnValue, void>
{
	std::function<void(TReturnValue)> funcComplete;

	AsyncOperand(std::function<TReturnValue(void)>&& func, std::function<void(TReturnValue)>&& funcComplete) :
		AsyncOperand<TReturnValue, void>(std::move(func)),
		funcComplete(funcComplete)
	{ }
};

struct AsyncTypeTag
{
	struct AsyncVoid 	{ };
	struct AsyncNonVoid { };
};

template<typename T>
struct AsyncTypeSelector
{
	typedef AsyncTypeTag::AsyncNonVoid Tag;
	typedef T EventDataType;
	typedef std::function<void(T)> CompleteFunction;
};

template<>
struct AsyncTypeSelector<void>
{
	typedef AsyncTypeTag::AsyncVoid Tag;
	typedef void* EventDataType;
	typedef std::function<void()> CompleteFunction;
};

template<typename T>
struct AsyncResult
{
	typedef T ReturnType;
	T value;

	AsyncResult() : value(T()) { };
};

template<>
struct AsyncResult<void>
{

};

template<typename T>
struct AsyncPackage
{
	std::function<T(void)> function;

	SharedEventObject<AsyncTask<T>*, typename AsyncTypeSelector<T>::EventDataType> event;
	typename AsyncTypeSelector<T>::CompleteFunction funcComplete;


	void(*completeFunc)(void*);
	AsyncResult<T> result;
	void* taskHandle;
	bool awaitable;

	AsyncPackage() : taskHandle(nullptr), awaitable(true), completeFunc(nullptr) { };
};




template<typename T>
void AsyncWorker(void* package)
{
	auto ptr = reinterpret_cast<AsyncPackage<T>*>(package);
	ptr->result.value = ptr->function();
}

template<>
inline void AsyncWorker<void>(void* package)
{
	auto ptr = reinterpret_cast<AsyncPackage<void>*>(package);
	ptr->function();
}

template<typename T>
void AsyncCompleteHandler(void* package)
{
	AsyncPackage<T>* ptr = reinterpret_cast<AsyncPackage<T>*>(package);
	ptr->event(reinterpret_cast<AsyncTask<T>*>(ptr->taskHandle), ptr->result.value);
}

template<>
void AsyncCompleteHandler<void>(void* package)
{
	AsyncPackage<void>* ptr = reinterpret_cast<AsyncPackage<void>*>(package);
	ptr->event(reinterpret_cast<AsyncTask<void>*>(ptr->taskHandle), nullptr);

}

template<typename T>
void AsyncCompleteHandlerComplete(void* package)
{
	AsyncPackage<T>* ptr = reinterpret_cast<AsyncPackage<T>*>(package);
	ptr->funcComplete(ptr->result.value);
}

template<>
void AsyncCompleteHandlerComplete<void>(void* package)
{
	AsyncPackage<void>* ptr = reinterpret_cast<AsyncPackage<void>*>(package);
	ptr->funcComplete();
}

template<typename T>
void AsyncFinalizeHandler(void* package)
{
	auto ptr = reinterpret_cast<AsyncPackage<T>*>(package);
	delete ptr;
}


template<typename TReturnValue, typename TEvent>
auto PackOperand(AsyncOperand<TReturnValue, TEvent> const& operand)
	-> typename std::enable_if<!std::is_void<TEvent>::value, AsyncPackage<TReturnValue>*>::type
{
	AsyncPackage<TReturnValue>* ret = new AsyncPackage<TReturnValue>;
	ret->function = operand.asyncFunc;
	ret->event = operand.eventRef;
	ret->completeFunc = AsyncCompleteHandler<TReturnValue>;
	ret->awaitable = false;
	return ret;
}

template<typename TReturnValue, typename TLambdaComplete>
auto PackOperand(AsyncOperand<TReturnValue, AsyncCompleteOperand<TLambdaComplete>> const& operand)
	-> AsyncPackage<TReturnValue>*
{
	AsyncPackage<TReturnValue>* ret = new AsyncPackage<TReturnValue>;
	ret->function = operand.asyncFunc;
	ret->funcComplete = operand.funcComplete;
	ret->completeFunc = AsyncCompleteHandlerComplete<TReturnValue>;
	ret->awaitable = false;
	return ret;
}

template<typename TReturnValue>
auto PackOperand(AsyncOperand<TReturnValue, void> const& operand)
	-> AsyncPackage<TReturnValue>*
{
	AsyncPackage<TReturnValue>* ret = new AsyncPackage<TReturnValue>;
	ret->function = operand.asyncFunc;
	return ret;
}

struct AsyncHandlerPayload
{
	typedef void(*FunctionType)(void*);
	FunctionType taskFunc;
	FunctionType completeFunc;
	FunctionType finalizeFunc;
	void*		 internalData;
	void**		 taskHandleRef;
	bool		 awaitable;
};

void* RunAsyncTask(AsyncHandlerPayload payload);

void AwaitAsyncTask(void* handle, void*& package, bool& doFinalize);

struct AwaitBuilder
{
	template<typename TReturnValue>
	TReturnValue operator&(AsyncTask<TReturnValue>* taskHandle)
	{
		void* packagePtr = nullptr;
		bool doFinalize = false;
		AwaitAsyncTask(taskHandle, packagePtr, doFinalize);

		if(doFinalize)
		{
			auto package = reinterpret_cast<AsyncPackage<TReturnValue>*>(packagePtr);
			TReturnValue ret = package->result.value;
			delete package;
			return ret;
		}
		else
		{
			return reinterpret_cast<AsyncPackage<TReturnValue>*>(packagePtr)->result.value;
		}
	}

	void operator&(AsyncTask<void>* taskHandle)
	{
		bool doFinalize = false;
		void* packagePtr = nullptr;
		AwaitAsyncTask(taskHandle, packagePtr, doFinalize);

		if(doFinalize)
		{
			delete reinterpret_cast<AsyncPackage<void>*>(packagePtr);
		}
	}
};

struct AsyncBuilder
{
	template<typename TReturnValue, typename TEvent>
	auto operator& (AsyncOperand<TReturnValue, TEvent> operand)
		-> AsyncTask<TReturnValue>*
	{
		auto packed = PackOperand(operand);

		AsyncHandlerPayload payload = {
			AsyncWorker<TReturnValue>,
			packed->completeFunc,
			AsyncFinalizeHandler<TReturnValue>,
			packed,
			&packed->taskHandle,
			packed->awaitable
		};
		return reinterpret_cast<AsyncTask<TReturnValue>*>(RunAsyncTask(payload));
	}

	template<typename TLambda,
			 typename TIntrospect = Introspect::CallableObject<TLambda>>
	auto operator&(TLambda lambda)
		-> AsyncTask<typename TIntrospect::ReturnType>*
	{
		return operator&(AsyncOperand<typename TIntrospect::ReturnType>(lambda));
	}
};

struct CompleteBuilder
{
	template<typename TLambda>
	auto operator*(TLambda lambda)
		-> AsyncCompleteOperand<TLambda>
	{
		return AsyncCompleteOperand<TLambda>(lambda);
	}
};

template<typename TLambda,
		 typename TIntrospect = Introspect::CallableObject<TLambda>,
		 typename 			  = typename std::enable_if<TIntrospect::Arity == 0>::type>
auto operator>>(TLambda lambda, typename AsyncOperand<typename TIntrospect::ReturnType>::EventType& event)
	-> AsyncOperand<typename TIntrospect::ReturnType, typename AsyncOperand<typename TIntrospect::ReturnType>::EventType>
{
	return { lambda, event };
}

template<typename TLambdaAsync,
		 typename TLambdaAfter,
		 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
		 typename TIntrospectAfter = Introspect::CallableObject<TLambdaAfter>,
		 typename				   = typename std::enable_if<TIntrospectAsync::Arity == 0
		 	 	 	 	 	 	 	 	 	 	 	 	 	 && TIntrospectAfter::Arity == 1
		 	 	 	 	 	 	 	 	 	 	 	 	 	 && std::is_same<typename TIntrospectAfter::template Args<0>,
		 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 typename TIntrospectAsync::ReturnType>::value>>
auto operator>>(TLambdaAsync async, AsyncCompleteOperand<TLambdaAfter> after)
	-> AsyncOperand<typename TIntrospectAsync::ReturnType, AsyncCompleteOperand<TLambdaAfter>>
{
	return { async, std::move(after.CompleteLambda) };
}



}}}

template<typename TReturnValue>
struct TFC::Async
{
	typedef Core::Async::AsyncTask<TReturnValue> Task;
	typedef Core::SharedEventObject<Task*,
									typename std::conditional<std::is_void<TReturnValue>::value,
															  void*,
															  TReturnValue>::type
									> Event;
	typedef typename Event::Type BaseEvent;

};

/**
 * @page tfc-async TFC Asynchronous Library
 * tfc_async is keyword to declare asynchronous block. Asynchronous block is processed on separate
 * thread which does not block the main thread, thus not blocking the entire application event
 * loop. Asynchronous block is suitable if program requires processing that takes longer which may
 * halt the user interaction during its execution.
 *
 * Asynchronous block can return any value which will be passed to completion handler. Completion
 * handler can be a SharedEvent or a tfc_async_complete block. Completion handler is guaranteed to
 * be performed synchronously after asynchronous block is completed.
 *
 * Due to the nature of lambda closure and C++ expression statement to build this mechanism,
 * unfortunately every tfc_async declaration must be concluded with an awkward semicolon.
 *
 * Basic usage with tfc_async_complete block:
 * ```
 * void ListController::GetFromServer()
 * {
 *    tfc_async
 *    {
 *        return PerformHttpRequestWithCurl();
 *    }
 *    tfc_async_complete(std::string result)
 *    {
 *        evas_object_text_set(labelStatus, result.c_str());
 *    };
 * }
 * ```
 *
 * Usage with event-based completion handler:
 * ```
 * class TheClass
 * {
 * private:
 *     Evas_Object* labelStatus;
 *     TFC::Async<int>::Event eventAsyncCompleted;
 *
 *     void OnAsyncCompleted(TFC::Async<int>::BaseEvent* ev, TFC::Async<int>::Task* t, int res)
 *     {
 *         evas_object_text_set(labelStatus, "Async completed!");
 *     }
 *
 * public:
 *     TheClass()
 *     {
 *         this->eventAsyncCompleted += EventHandler(TheClass::OnAsyncCompleted);
 *     }
 *
 *     void RunTask()
 *     {
 *         tfc_async
 *         {
 *             int val = rand();
 *             return rand * 3;
 *         } >> eventAsyncCompleted;
 *     }
 * };
 * ```
 *
 * tfc_async returns a task handle which can be used to refer the running task. The task handle type
 * is pointer to @see{TFC::Core::AsyncTask} or @see{TFC::Async::Task} as template trait equivalence.
 * Task handle is used to identify asynchronous type and will remain valid as long as the thread is
 * running.
 *
 * Task handle can be used to perform blocking tfc_await or non-blocking tfc_try_await. Those
 * operation can only be performed if tfc_async is called without completion handler. Task handle
 * can also useful to identify running task on event-based completion handler, if a handler is
 * supposed to handle completion of multiple running task. Using tfc_await and tfc_try_await
 * in a tfc_async with completion handler will result in undefined behavior.
 *
 * Retrieving task handle on tfc_async:
 * ```
 * class TheClass
 * {
 * private:
 *     TFC::Async<int>::Task taskCompute;
 * public:
 *     void RunTask()
 *     {
 *         this->taskCompute = tfc_async {
 *             int a = rand();
 *             return a * 2;
 *         };
 *     }
 * };
 * ```
 *
 */
#define tfc_async TFC::Core::Async::AsyncBuilder() & [=] ()
#define tfc_await TFC::Core::Async::AwaitBuilder() &
#define tfc_try_await
#define tfc_async_complete >> TFC::Core::Async::CompleteBuilder() * [=]
#define tfc_if_abort_return

#endif /* ASYNC_NEW_H_ */
