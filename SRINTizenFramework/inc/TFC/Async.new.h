/*
 * Async.new.h
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
};

template<>
struct AsyncTypeSelector<void>
{
	typedef AsyncTypeTag::AsyncVoid Tag;
	typedef void* EventDataType;
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
	AsyncResult<T> result;
	SharedEventObject<AsyncTask<T>*, typename AsyncTypeSelector<T>::EventDataType> event;
	std::function<T(void)> function;
};

template<typename TReturnValue, typename TEvent>
AsyncPackage<TReturnValue>* PackOperand(AsyncOperand<TReturnValue, TEvent> operand)
{

}

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
	std::unique_ptr<AsyncPackage<T>> ptr(reinterpret_cast<AsyncPackage<T>*>(package));
}

template<>
void AsyncCompleteHandler<void>(void* package)
{
	std::unique_ptr<AsyncPackage<void>> ptr(reinterpret_cast<AsyncPackage<void>*>(package));
}

template<typename T>
void AsyncFinalizeHandler(void* package)
{
	auto ptr = reinterpret_cast<AsyncPackage<T>*>(package);
	delete ptr;
}

struct AsyncHandlerPayload
{
	typedef void(*FunctionType)(void*);

	FunctionType taskFunc;
	FunctionType completeFunc;
	FunctionType finalizeFunc;
	void*		 internalData;
};

void* RunAsyncTask(AsyncHandlerPayload payload);

void* RunAsyncTask(void(*task)(void*), void(*complete)(void*), void* package);

void AwaitAsyncTask(void* handle, void** package);

struct AwaitBuilder
{
	template<typename TReturnValue>
	TReturnValue operator&(AsyncTask<TReturnValue>* taskHandle)
	{
		void* packagePtr = nullptr;
		AwaitAsyncTask(taskHandle, &packagePtr);
		return reinterpret_cast<AsyncPackage<TReturnValue>*>(packagePtr)->result.value;
	}

	void operator&(AsyncTask<void>* taskHandle)
	{
		AwaitAsyncTask(taskHandle, nullptr);
	}
};

struct AsyncBuilder
{
	template<typename TReturnValue, typename TEvent>
	auto operator& (AsyncOperand<TReturnValue, TEvent> operand)
		-> AsyncTask<TReturnValue>*
	{
		return reinterpret_cast<AsyncTask<TReturnValue>*>(
										RunAsyncTask(AsyncWorker<TReturnValue>,
													 AsyncCompleteHandler<TReturnValue>));
	}

	template<typename TLambda,
			 typename TIntrospect = Introspect::CallableObject<TLambda>>
	auto operator&(TLambda lambda)
		-> AsyncTask<typename TIntrospect::ReturnType>*
	{
		return operator&(AsyncOperand<typename TIntrospect::ReturnType>(lambda));
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

};

inline void TFC::Core::Async::AwaitAsyncTask(void* handle, void** package) {
}

#define tfc_async
#define tfc_await
#define tfc_if_abort_return

#endif /* ASYNC_NEW_H_ */
