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
 *    Async.h
 *
 * Asynchronous infrastructure to perform seamless integration between
 * synchronous code and asynchronous code with the aid of Embedded
 * Domain-Specific Language (EDSL).
 *
 * Created on: 	 Oct 25, 2016
 * Author: 		 Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributors: Kevin Winata (k.winata@samsung.com)
 */

#pragma once

#ifndef TFC_CORE_ASYNC_NEW_H_
#define TFC_CORE_ASYNC_NEW_H_

#include <utility>
#include <dlog.h>
#include "TFC/Core.h"
#include "TFC/Core/Introspect.h"
#include "TFC/Core/Metaprogramming.h"

namespace TFC {

template<typename TReturnValue>
struct Async;


namespace Core {
namespace Async {

template<typename TReturnValue>
struct AsyncTask;

/**
 * Base class template for AsyncOperand of tfc_async syntax. AsyncOperand encapsulates asynchronous syntax tree
 * to be processed by AsyncBuilder. AsyncOperand contains information about lambda closure for asynchronous
 * operation, as well as additional information such as complete handler or complete event on its specialization.
 *
 * This base template defines AsyncOperand for asynchronous operation which returns some value but does not have
 * completion notification.
 */
template<
	typename TLambda,
	typename TCompleteType = void,
	typename TReturnValue = typename Introspect::CallableObject<TLambda>::ReturnType
>
struct AsyncOperand
{
	typedef TReturnValue											  ReturnType;
	typedef AsyncTask<TReturnValue>									  AsyncTaskType;
	typedef SharedEventObject<AsyncTask<TReturnValue>*, TReturnValue> EventType;
	TLambda asyncFunc;

	AsyncOperand(TLambda&& aLambda) :
		asyncFunc(std::move(aLambda))
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Build async operand");
	}
};

/**
 * Specialization for AsyncOperand where the asynchronous operation does not return any value and does not
 * have completion notification.
 */
template<typename TLambda>
struct AsyncOperand<TLambda, void, void>
{
	typedef void											ReturnType;
	typedef AsyncTask<void>									AsyncTaskType;
	typedef SharedEventObject<AsyncTask<void>*, void*> 		EventType;

	TLambda asyncFunc;

	AsyncOperand(TLambda&& aLambda) :
		asyncFunc(std::move(aLambda))
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Build async operand");
	}
};

/**
 * Specialization for AsyncOperand where the asynchronous operation does not return any value and have
 * completion notification via event.
 */
template<typename TLambda>
struct AsyncOperand<TLambda, SharedEventObject<AsyncTask<void>*, void*>, void> :
	AsyncOperand<TLambda, void, void>
{
	typename AsyncOperand<TLambda, void, void>::EventType&	eventRef;

	AsyncOperand(TLambda&& func, typename AsyncOperand<TLambda>::EventType& eventRef) :
		AsyncOperand<TLambda, void, void>(std::move(func)),
		eventRef(eventRef)
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Build async operand with function object");
	}
};

/**
 * Specialization for AsyncOperand where the asynchronous operation return a value and have completion
 * notification via event.
 */
template<typename TLambda, typename TReturnValue>
struct AsyncOperand<TLambda, SharedEventObject<AsyncTask<TReturnValue>*, TReturnValue>, TReturnValue> :
	AsyncOperand<TLambda, void, TReturnValue>
{

	typename AsyncOperand<TLambda, void, TReturnValue>::EventType&	eventRef;

	AsyncOperand(TLambda&& func, typename AsyncOperand<TLambda>::EventType& eventRef) :
		AsyncOperand<TLambda, void, TReturnValue>(std::move(func)),
		eventRef(eventRef)
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Build async operand with function object");
	}
};

/**
 * Base class template for AsyncCompleteOperand which wraps the syntax tree of tfc_async_complete clause.
 * It contains information on lambda closure for asynchronous complete operation and specializes on
 * its parameter. The parameter on lambda closure needs to be matched with the asynchronous lambda return type.
 * But because tfc_async_complete is evaluated earlier before tfc_async itself, The checking is delayed until
 * tfc_async is evaluated.
 *
 * This base template defines a tfc_async_complete closure with non-void parameter.
 */
template<typename TLambda,
		 typename TCatchList = std::tuple<>,
		 typename TIntrospect = Introspect::CallableObject<TLambda>,
		 bool TNonVoidParam   = TIntrospect::Arity == 1,
		 bool TParamOneOrZero = TIntrospect::Arity <= 1>
struct AsyncCompleteOperand
{
	typedef typename Introspect::CallableObject<TLambda>::template Args<0> ArgumentType;
	static constexpr bool Valid = true;
	static constexpr bool IsVoid = false;

	TLambda completeLambda;
	bool catchListValid;
	TCatchList catchList;

	AsyncCompleteOperand(TLambda&& completeLambda) :
		completeLambda(std::move(completeLambda)), catchListValid(false)
	{

	}

	AsyncCompleteOperand(TLambda&& completeLambda, TCatchList&& theList) :
		completeLambda(std::move(completeLambda)), catchListValid(true), catchList(std::move(theList))
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Create AsyncCompleteOperand");
	}

	static AsyncCompleteOperand<TLambda> MakeOperand(TLambda&& lambda)
	{
		return { std::move(lambda) };
	}

	template<typename TLambdaAsync,
			 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
			 bool 	  RuleValidation   = std::is_same<typename std::remove_reference<typename TIntrospect::template Args<0>>::type,
								  	  	  	  	  	  typename TIntrospectAsync::ReturnType>::value>
	struct Match
	{
		static constexpr bool Valid = RuleValidation;
	};
};

extern std::tuple<> emptyTuple;

/**
 * Specialization for AsyncCompleteOperand with void parameter.
 */
template<typename TLambda, typename TCatchList, typename TIntrospect>
struct AsyncCompleteOperand<TLambda, TCatchList, TIntrospect, false, true>
{
	static constexpr bool Valid = true;
	static constexpr bool IsVoid = true;

	TLambda completeLambda;
	bool catchListValid;
	TCatchList catchList;

	AsyncCompleteOperand(TLambda&& completeLambda) :
		completeLambda(std::forward<TLambda>(completeLambda)), catchListValid(false), catchList(std::move(emptyTuple))
	{

	}

	AsyncCompleteOperand(TLambda&& completeLambda, TCatchList&& theList) :
		completeLambda(std::move(completeLambda)), catchListValid(true), catchList(std::move(theList))
	{

	}

	static AsyncCompleteOperand<TLambda> MakeOperand(TLambda&& lambda)
	{
		return { std::forward<TLambda>(lambda) };
	}

	template<typename TLambdaAsync,
			 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
			 bool 	  RuleValidation   = std::is_same<void,
									  	  	  	  	  typename TIntrospectAsync::ReturnType>::value>
	struct Match
	{
		static constexpr bool Valid = RuleValidation;
	};
};

/**
 * Specialization for fallback of incorrect lambda type, where it has more than one parameter. This
 * is to support assertion in later evaluation.
 */
template<typename TLambda, typename TCatchList, typename TIntrospect>
struct AsyncCompleteOperand<TLambda, TCatchList, TIntrospect, false, false>
{
	static constexpr bool Valid = false;

	explicit AsyncCompleteOperand()
	{
	}

	template<typename TAny>
	static AsyncCompleteOperand<TLambda> MakeOperand(TAny lambda)
	{
		return { };
	}
};

/**
 * Specialization of AsyncOperand which uses completion notification via inline lambda with tfc_async_complete
 * syntax.
 */
template<typename TLambda, typename TReturnValue, typename TLambdaComplete, typename TCatchList>
struct AsyncOperand<TLambda, AsyncCompleteOperand<TLambdaComplete, TCatchList>, TReturnValue> :
	AsyncOperand<TLambda, void, TReturnValue>
{
	AsyncCompleteOperand<TLambdaComplete, TCatchList>&& completeOperand;

	AsyncOperand(TLambda&& func, AsyncCompleteOperand<TLambdaComplete, TCatchList>&& completeOperand) :
		AsyncOperand<TLambda, void, TReturnValue>(std::forward<TLambda>(func)),
		completeOperand(std::move(completeOperand))
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Async operand with lambda complete constructor");
	}
};

/**
 * Container class to store asynchronous result.
 */
template<typename T, bool = std::is_move_constructible<T>::value>
struct AsyncResult
{
	typedef T ReturnType;
	T value;

	AsyncResult(T&& value) : value(std::move(value)) { };
};

template<typename T>
struct AsyncResult<T, false>
{
	typedef T ReturnType;
	T value;
	AsyncResult(T&& value) : value(value) { };
};

/**
 * Specialization if the asynchronous operation does not return any value.
 */
template<>
struct AsyncResult<void>
{

};

//// FORWARD DECLARATION

template<typename TLambda, typename TReturnType = typename Introspect::CallableObject<TLambda>::ReturnType>
struct CompleteLambdaInvoker;

template<typename TLambda, typename TReturnType = typename Introspect::CallableObject<TLambda>::ReturnType>
struct CompleteEventInvoker;

//// END OF FORWARD DECLARATION

/**
 * Template class for AsyncTaskInterface which stips down information to the necessary minimal information
 * required in user codes which is the result value of asynchronous operation. This class is necessary because
 * the template performs type-erasure, while internally the asynchronous engine is templateless, it needs to
 * know necessary information how to delete the information passed to it.
 *
 * To ensure the bridging between user codes and internal asynchronous engine, this class will act as interface
 * without requiring the user codes to define the lambda type on the template. So the user codes may see the
 * opaque pointer of AsyncTask<TReturnValue> which still carry the return type information.
 */
template<typename TReturnType>
struct AsyncTaskInterface
{
	typedef void(DeleterFunc)(void*);

	AsyncResult<TReturnType>* result;
	DeleterFunc* deleterFunction;

	AsyncTaskInterface(DeleterFunc* deleterFunction) : result(nullptr), deleterFunction(deleterFunction) { }

	~AsyncTaskInterface()
	{
		if(result != nullptr)
			delete result;
	}
};

/**
 * Base class template for AsyncCompletePackage which wraps the completion lambda so it can be called
 * via asyncrhonous engine which is templateless. This class actually acts similar with polymorphism,
 * but it does not utilizes C++ polymorphism to reduce unnecessary virtual table creation as each
 * instantiation of this class will (may) be unique to each lambda. This class also behaves similar
 * with std::function (as this class is actually replacing the std::function) but this class is a more
 * lightweight version of std::function which can only be used with lambda type.
 */
template<typename TReturnType>
struct AsyncCompletePackage
{
	/**
	 * Inner class which wraps the completion lambda itself
	 */
	template<typename TLambdaComplete>
	struct LambdaStorage
	{
		TLambdaComplete lambdaStorage;

		explicit LambdaStorage(TLambdaComplete&& l) :
			lambdaStorage(std::move(l))
		{

		}

		struct NonMovableTag { };
		struct MovableTag { };

		static void InvokerStatic(LambdaStorage<TLambdaComplete>* thiz, TReturnType&& val, NonMovableTag)
		{
			thiz->lambdaStorage(val);
		}

		static void InvokerStatic(LambdaStorage<TLambdaComplete>* thiz, TReturnType&& val, MovableTag)
		{
			thiz->lambdaStorage(std::move(val));
		}

		/**
		 * Static function which will invoke the lambda on behalf of its caller by assuming that the
		 * storage parameter is exactly the correct LambdaStorage type.
		 */
		static void Invoker(void* storage, TReturnType&& val)
		{
			// Determine whether the TReturnType is movable or not, as both kind have different calling convention especially if the
			// parameter is not r-value reference.
			typedef typename std::conditional<std::is_move_constructible<TReturnType>::value, MovableTag, NonMovableTag>::type MovabilityTag;

			auto thiz = reinterpret_cast<LambdaStorage<TLambdaComplete>*>(storage);
			InvokerStatic(thiz, std::move(val), MovabilityTag {});
		}

		/**
		 * Static function which will invoke the deletion on the lambda storage on behalf of its caller
		 * by assuming that the storage parameter is exactly the correct LambdaStorage type.
		 */
		static void Deleter(void* storage)
		{
			auto thiz = reinterpret_cast<LambdaStorage<TLambdaComplete>*>(storage);
			delete thiz;
		}

		/**
		 * Pack lambda into a lambda storage.
		 */
		static void* PackLambda(TLambdaComplete&& l)
		{
			return new LambdaStorage { std::move(l) };
		}
	};

	void* storage; 									/**< Pointer to store the LambdaStorage pointer */
	void (*invokerFunc)(void*, TReturnType&& val); 	/**< Pointer to the invoker function of the LambdaStorage */
	void (*deleterFunc)(void*); 					/**< Pointer to the deleter function of the LambdaStorage */

	/**
	 * Invoke the call to the completion lambda.
	 */
	void operator()(TReturnType&& val)
	{
		if(storage != nullptr)
			invokerFunc(storage, std::move(val));
	}

	/**
	 * Creates empty AsyncCompletePackage.
	 */
	AsyncCompletePackage() :
		storage(nullptr),
		invokerFunc(nullptr),
		deleterFunc(nullptr)
	{

	}

	/**
	 * Creates AsyncCompletePackage with the completion lambda.
	 */
	template<typename TLambdaComplete>
	AsyncCompletePackage(TLambdaComplete&& lambda) :
		storage(LambdaStorage<TLambdaComplete>::PackLambda(std::move(lambda))),
		invokerFunc(LambdaStorage<TLambdaComplete>::Invoker),
		deleterFunc(LambdaStorage<TLambdaComplete>::Deleter)
	{

	}

	/**
	 * Destructor of AsyncCompletePackage which also destruct the LambdaStorage.
	 */
	~AsyncCompletePackage()
	{
		if(storage != nullptr)
			deleterFunc(storage);
	}
};

template<>
struct AsyncCompletePackage<void>
{
	template<typename TLambdaComplete>
	struct LambdaStorage
	{
		TLambdaComplete lambdaStorage;

		explicit LambdaStorage(TLambdaComplete&& l) :
			lambdaStorage(std::move(l))
		{

		}

		static void Invoker(void* storage)
		{
			auto thiz = reinterpret_cast<LambdaStorage<TLambdaComplete>*>(storage);
			thiz->lambdaStorage();
		}

		static void Deleter(void* storage)
		{
			auto thiz = reinterpret_cast<LambdaStorage<TLambdaComplete>*>(storage);
			delete thiz;
		}

		static void* PackLambda(TLambdaComplete&& l)
		{
			return new LambdaStorage { std::move(l) };
		}
	};
	void* storage;
	void (*invokerFunc)(void*);
	void (*deleterFunc)(void*);

	void operator()()
	{
		if(storage != nullptr)
			invokerFunc(storage);
	}

	AsyncCompletePackage() :
		storage(nullptr),
		invokerFunc(nullptr),
		deleterFunc(nullptr)
	{

	}

	template<typename TLambdaComplete>
	AsyncCompletePackage(TLambdaComplete&& lambda) :
		storage(LambdaStorage<TLambdaComplete>::PackLambda(std::move(lambda))),
		invokerFunc(LambdaStorage<TLambdaComplete>::Invoker),
		deleterFunc(LambdaStorage<TLambdaComplete>::Deleter)
	{

	}

	~AsyncCompletePackage()
	{
		if(storage != nullptr)
			deleterFunc(storage);
	}
};

template<typename TLambda>
struct AsyncPackage : public AsyncTaskInterface<typename Introspect::CallableObject<TLambda>::ReturnType>
{
	typedef typename Introspect::CallableObject<TLambda>::ReturnType ReturnType;

	TLambda functionAsync;

	typename AsyncOperand<TLambda>::EventType eventComplete;
	AsyncCompletePackage<ReturnType> lambdaComplete;

	void(*completeInvoker)(void*);
	void* taskHandle;
	bool awaitable;

	static void DeleterFunction(void* thiz)
	{
		delete reinterpret_cast<AsyncPackage<TLambda>*>(thiz);
	}

	AsyncPackage() : completeInvoker(nullptr), taskHandle(nullptr), awaitable(true) { };

	AsyncPackage(TLambda&& functionAsync) :
				AsyncTaskInterface<ReturnType>(DeleterFunction),
				functionAsync(std::move(functionAsync)),
				eventComplete(nullptr),
				completeInvoker(nullptr),
				taskHandle(nullptr),
				awaitable(true)
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "AsyncPackage constructor only async");
	}

	AsyncPackage(
			TLambda&& functionAsync,
			typename AsyncOperand<TLambda>::EventType eventComplete) :
				AsyncTaskInterface<ReturnType>(DeleterFunction),
				functionAsync(std::move(functionAsync)),
				eventComplete(eventComplete),
				completeInvoker(CompleteEventInvoker<TLambda>::Func),
				taskHandle(nullptr),
				awaitable(false)
	{

	}

	template<typename TLambdaComplete>
	AsyncPackage(
			TLambda&& functionAsync,
			TLambdaComplete&& lambdaComplete) :
				AsyncTaskInterface<ReturnType>(DeleterFunction),
				functionAsync(std::move(functionAsync)),
				eventComplete(nullptr),
				lambdaComplete(std::move(lambdaComplete)),
				completeInvoker(CompleteLambdaInvoker<TLambda>::Func),
				taskHandle(nullptr),
				awaitable(false)
	{

	}

	~AsyncPackage()
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Async Package deleted");
	}
};

template<typename TLambda, typename TReturnType = typename Introspect::CallableObject<TLambda>::ReturnType>
struct AsyncWorker
{
	static void Func(void* package, void* taskHandle)
	{
		auto ptr = reinterpret_cast<AsyncPackage<TLambda>*>(package);
		ptr->result = new AsyncResult<TReturnType>(ptr->functionAsync(taskHandle));
	}
};

template<typename TLambda>
struct AsyncWorker<TLambda, void>
{
	static void Func(void* package, void* taskHandle)
	{
		auto ptr = reinterpret_cast<AsyncPackage<TLambda>*>(package);
		ptr->functionAsync(taskHandle);
	}
};


template<typename TLambda, typename TReturnType>
struct CompleteEventInvoker
{
	static void Func(void* package)
	{
		auto ptr = reinterpret_cast<AsyncPackage<TLambda>*>(package);
		ptr->eventComplete(reinterpret_cast<AsyncTask<TReturnType>*>(ptr->taskHandle), ptr->result->value);
	}
};

template<typename TLambda>
struct CompleteEventInvoker<TLambda, void>
{
	static void Func(void* package)
	{
		auto ptr = reinterpret_cast<AsyncPackage<TLambda>*>(package);
		ptr->eventComplete(reinterpret_cast<AsyncTask<void>*>(ptr->taskHandle), nullptr);
	}
};

template<typename TLambda, typename TReturnType>
struct CompleteLambdaInvoker
{
	static void Func(void* package)
	{
		AsyncPackage<TLambda>* ptr = reinterpret_cast<AsyncPackage<TLambda>*>(package);
		ptr->lambdaComplete(std::move(ptr->result->value));
	}
};

template<typename TLambda>
struct CompleteLambdaInvoker<TLambda, void>
{
	static void Func(void* package)
	{
		auto ptr = reinterpret_cast<AsyncPackage<TLambda>*>(package);
		ptr->lambdaComplete();
	}
};

template<typename TLambda, typename TEvent, typename TReturnValue>
auto PackOperand(AsyncOperand<TLambda, TEvent, TReturnValue>&& operand)
	-> typename std::enable_if<!std::is_void<TEvent>::value, AsyncPackage<TLambda>*>::type
{
	dlog_print(DLOG_DEBUG, "TFC-Debug", "Pack operand with event");
	return new AsyncPackage<TLambda> { std::forward<TLambda>(operand.asyncFunc), operand.eventRef };
}

template<typename TLambda, typename TLambdaComplete, typename TReturnValue, typename TCatchList>
auto PackOperand(AsyncOperand<TLambda, AsyncCompleteOperand<TLambdaComplete, TCatchList>, TReturnValue>&& operand)
	-> AsyncPackage<TLambda>*
{
	dlog_print(DLOG_DEBUG, "TFC-Debug", "Pack operand with complete");
	return new AsyncPackage<TLambda> { std::forward<TLambda>(operand.asyncFunc), std::move(operand.completeOperand.completeLambda) };
}

template<typename TLambda>
auto PackOperand(AsyncOperand<TLambda, void>&& operand)
	-> AsyncPackage<TLambda>*
{
	dlog_print(DLOG_DEBUG, "TFC-Debug", "Pack operand no complete");
	return new AsyncPackage<TLambda> { std::forward<TLambda>(operand.asyncFunc) };
}

struct CatchInvoker
{
	typedef void(CatchFunctionType)(void*, void*);
	bool handled;
	CatchFunctionType* catchFunc;
	void* data;
	void* ex;

	CatchInvoker() : handled(false), catchFunc(nullptr), data(nullptr), ex(nullptr) { }
	CatchInvoker(CatchFunctionType* f, void* data, void* ex) : handled(true), catchFunc(f), data(data), ex(ex) { }

	void InvokeHandler()
	{
		if(handled)
			catchFunc(data, ex);
	}
};

struct AsyncHandlerPayload
{
	typedef void	(FunctionType)		(void*);
	typedef void	(AsyncFunctionType)	(void*, void*);

	AsyncFunctionType* 	taskFunc;
	FunctionType* 		completeInvoker;
	FunctionType* 		finalizeFunc;
	void*		 		internalData;
	void**				taskHandleRef;
	bool		 		awaitable;

	typedef CatchInvoker (CatchHandlerFunctionType) (std::exception const&, void*);
	CatchHandlerFunctionType* 	catchHandler;
	void*						catchHandlerData;
	FunctionType*				catchHandlerFinalizeFunc;
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
			dlog_print(DLOG_DEBUG, "TFC-Debug", "Do finalize");
			auto package = reinterpret_cast<AsyncTaskInterface<TReturnValue>*>(packagePtr);
			TReturnValue ret = package->result->value;
			auto deleter = reinterpret_cast<AsyncTaskInterface<void>*>(packagePtr)->deleterFunction;
			deleter(packagePtr);
			dlog_print(DLOG_DEBUG, "TFC-Debug", "Do finalize completes");
			return ret;
		}
		else
		{
			dlog_print(DLOG_DEBUG, "TFC-Debug", "No do finalize");
			return reinterpret_cast<AsyncTaskInterface<TReturnValue>*>(packagePtr)->result->value;
		}
	}

	void operator&(AsyncTask<void>* taskHandle)
	{
		bool doFinalize = false;
		void* packagePtr = nullptr;
		AwaitAsyncTask(taskHandle, packagePtr, doFinalize);

		if(doFinalize)
		{
			auto deleter = reinterpret_cast<AsyncTaskInterface<void>*>(packagePtr)->deleterFunction;
			deleter(packagePtr);
		}
	}
};

template<typename TCatchList>
struct CatchHandler;

template<typename... TCatchList>
struct CatchHandlerLoop;

template<typename TEvent>
struct CatchHandlerPayloadSelector
{
	static constexpr AsyncHandlerPayload::CatchHandlerFunctionType* catchHandler = nullptr;
	static constexpr AsyncHandlerPayload::FunctionType* catchHandlerFinalizeFunc = nullptr;

	template<typename TAny>
	static void* Data(TAny& operand) { dlog_print(DLOG_DEBUG, "TFC-Debug", "Get None"); return nullptr; }
};

template<typename TCatchList>
struct CatchListDecay;

template<typename... TCatchOperands>
struct CatchListDecay<std::tuple<TCatchOperands...>>
{
	typedef std::tuple<typename std::remove_reference<TCatchOperands>::type...> TupleType;
};

template<typename TLambdaComplete, typename TCatchList>
struct CatchHandlerPayloadSelector<AsyncCompleteOperand<TLambdaComplete, TCatchList>>
{
	static void FinalizeFunc(void* data)
	{
		delete reinterpret_cast<CatchListType*>(data);
	}

	typedef typename CatchListDecay<TCatchList>::TupleType CatchListType;

	static constexpr AsyncHandlerPayload::CatchHandlerFunctionType* catchHandler = CatchHandler<CatchListType>::Func;
	static constexpr AsyncHandlerPayload::FunctionType* catchHandlerFinalizeFunc = FinalizeFunc;

	template<typename TLambda>
	static void* Data(AsyncOperand<TLambda, AsyncCompleteOperand<TLambdaComplete, TCatchList>>& operand)
	{
		if(!operand.completeOperand.catchListValid)
			return nullptr;

		dlog_print(DLOG_DEBUG, "TFC-Debug", "Get Data");
		return new CatchListType(std::move(operand.completeOperand.catchList));
	}
};

template<typename TLambdaComplete>
struct CatchHandlerPayloadSelector<AsyncCompleteOperand<TLambdaComplete, std::tuple<>>>
{
	static constexpr AsyncHandlerPayload::CatchHandlerFunctionType* catchHandler = nullptr;
	static constexpr AsyncHandlerPayload::FunctionType* catchHandlerFinalizeFunc = nullptr;

	template<typename TAny>
	static void* Data(TAny& operand) { dlog_print(DLOG_DEBUG, "TFC-Debug", "Get None"); return nullptr; }
};

struct AsyncBuilder
{
	template<typename TLambda, typename TEvent>
	auto operator& (AsyncOperand<TLambda, TEvent>&& operand)
		-> AsyncTask<typename AsyncOperand<TLambda>::ReturnType>*
	{
		typedef typename AsyncOperand<TLambda>::ReturnType TReturnValue;
		auto catchData = CatchHandlerPayloadSelector<TEvent>::Data(operand);
		dlog_print(DLOG_DEBUG, "TFC-Debug", "AFter catch getting data");
		auto packed = PackOperand(std::forward<AsyncOperand<TLambda, TEvent>>(operand));
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Build AsyncPayload");
		AsyncHandlerPayload payload = {
			AsyncWorker<TLambda>::Func,
			packed->completeInvoker,
			packed->deleterFunction,
			packed,
			&packed->taskHandle,
			packed->awaitable,
			CatchHandlerPayloadSelector<TEvent>::catchHandler, // Catch Handler
			catchData, // Catch Data
			CatchHandlerPayloadSelector<TEvent>::catchHandlerFinalizeFunc

		};
		return reinterpret_cast<AsyncTask<TReturnValue>*>(RunAsyncTask(payload));
	}

	template<typename TLambda,
			 typename TIntrospect = Introspect::CallableObject<TLambda>>
	auto operator&(TLambda&& lambda)
		-> AsyncTask<typename TIntrospect::ReturnType>*
	{
		return operator&(AsyncOperand<TLambda>(std::forward<TLambda>(lambda)));
	}
};

struct CompleteBuilder
{
	template<typename TLambda>
	auto operator*(TLambda&& lambda)
		-> AsyncCompleteOperand<TLambda>
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Pack Complete Operand");
		static_assert(AsyncCompleteOperand<TLambda>::Valid, "tfc_async_complete block has invalid result-capture declaration");
		return AsyncCompleteOperand<TLambda>::MakeOperand(std::move(lambda));
	}
};

template<typename TLambda,
		 typename TIntrospect = Introspect::CallableObject<TLambda>,
		 typename 			  = typename std::enable_if<TIntrospect::Arity == 1>::type>
auto operator>>(TLambda&& lambda, typename AsyncOperand<TLambda>::EventType& event)
	-> AsyncOperand<TLambda, typename AsyncOperand<TLambda>::EventType>
{
	dlog_print(DLOG_DEBUG, "TFC-Debug", "Build async with >> operator and event after");
	return { std::forward<TLambda>(lambda), event };
}

/*
The following template form :
	template<typename TLambdaAsync,
			 typename TLambdaAfter,
			 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
			 typename TIntrospectAfter = Introspect::CallableObject<TLambdaAfter>,
			 typename				   = typename std::enable_if<TIntrospectAsync::Arity == 0
																 && TIntrospectAfter::Arity == 1
																 && std::is_same<typename TIntrospectAfter::template Args<0>,
																				 typename TIntrospectAsync::ReturnType>::value>::type>
	auto operator>>(TLambdaAsync async, AsyncCompleteOperand<TLambdaAfter> after)
		-> AsyncOperand<typename TIntrospectAsync::ReturnType, AsyncCompleteOperand<TLambdaAfter>>
	{
		return { async, std::move(after.completeLambda) };
	}
is not applicable if we want to use std::enable_if and will result in [template parameter redefines default argument] error.
See : https://stackoverflow.com/questions/29502052/template-specialization-and-enable-if-problems
*/

template<typename TLambdaAsync,
		 typename TLambdaAfter,
		 typename TCatchList,
		 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
		 typename TIntrospectAfter = Introspect::CallableObject<TLambdaAfter>,
		 typename std::enable_if<AsyncCompleteOperand<TLambdaAfter>::template Match<TLambdaAsync>::Valid, int>::type* = nullptr>
auto operator>>(TLambdaAsync&& async, AsyncCompleteOperand<TLambdaAfter, TCatchList>&& after)
	-> AsyncOperand<TLambdaAsync, AsyncCompleteOperand<TLambdaAfter, TCatchList>>
{
	dlog_print(DLOG_DEBUG, "TFC-Debug", "Build async with >> operator and lambda after");
	return { std::forward<TLambdaAsync>(async), std::move(after) };
}


// Assert that CompleteBuilder >> operator will receive lambda with appropriate parameter
template<typename TLambdaAsync,
		 typename TLambdaAfter,
		 typename TCatchList,
		 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
		 typename TIntrospectAfter = Introspect::CallableObject<TLambdaAfter>,
		 typename std::enable_if<!AsyncCompleteOperand<TLambdaAfter>::Valid, int>::type* = nullptr>
auto operator>>(TLambdaAsync&& async, AsyncCompleteOperand<TLambdaAfter, TCatchList> after)
	-> AsyncOperand<TLambdaAsync, AsyncCompleteOperand<TLambdaAfter, TCatchList>>
{
	return { nullptr, nullptr };
}


template<typename TLambdaAsync,
		 typename TLambdaAfter,
		 typename TCatchList,
		 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
		 typename TIntrospectAfter = Introspect::CallableObject<TLambdaAfter>,
		 typename std::enable_if<!AsyncCompleteOperand<TLambdaAfter>::template Match<TLambdaAsync>::Valid, int>::type* = nullptr>
auto operator>>(TLambdaAsync&& async, AsyncCompleteOperand<TLambdaAfter, TCatchList> after)
	-> AsyncOperand<TLambdaAsync, AsyncCompleteOperand<TLambdaAfter, TCatchList>>
{
	static_assert(std::is_same<typename TIntrospectAfter::template Args<0>, typename TIntrospectAsync::ReturnType>::value,
			"Parameter for tfc_async_complete lambda must match with the return value from tfc_async.");
	return { nullptr, nullptr };
}

template<typename TLambdaSync>
struct SynchronizePackage
{
	TLambdaSync lambda;

	static void SynchronizeLambdaInvoker(void* package)
	{
		reinterpret_cast<SynchronizePackage<TLambdaSync>*>(package)->lambda();
	}

	static void Deleter(void* package)
	{
		delete reinterpret_cast<SynchronizePackage<TLambdaSync>*>(package);
	}

	SynchronizePackage(TLambdaSync&& param) : lambda(std::move(param)) { }
};

struct SynchronizeHandlerPayload
{
	typedef void(SynchronizePackageFunction)(void*);

	void* packagePtr;
	SynchronizePackageFunction* lambdaInvokerFunc;
	SynchronizePackageFunction* deleterFunc;

	template<typename TLambdaSync>
	static SynchronizeHandlerPayload* PackLambda(TLambdaSync&& l)
	{
		return new SynchronizeHandlerPayload {
			new SynchronizePackage<TLambdaSync>(std::move(l)),
			SynchronizePackage<TLambdaSync>::SynchronizeLambdaInvoker,
			SynchronizePackage<TLambdaSync>::Deleter
		};
	}

	~SynchronizeHandlerPayload() { if(packagePtr != nullptr) deleterFunc(packagePtr); }
};

void SynchronizeCall(void* taskHandle, SynchronizeHandlerPayload* p);

struct SynchronizeBuilder
{
	void* taskHandle;

	SynchronizeBuilder(void* taskHandle) : taskHandle(taskHandle)
	{

	}

	template<typename TLambdaSync>
	void operator&(TLambdaSync&& l)
	{
		SynchronizeCall(taskHandle, SynchronizeHandlerPayload::PackLambda(std::move(l)));
	}
};

///// tfc_async_catch mechanism

template<typename TLambda>
struct CatchOperand
{
	TLambda lambdaStorage;
	typedef typename std::decay<typename Introspect::CallableObject<TLambda>::template Args<0>>::type ExceptionArgs;

	static bool CanHandle(std::exception const& ex)
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Test %s <=> %s", typeid(ExceptionArgs).name(), typeid(ex).name());
		auto ptr = dynamic_cast<ExceptionArgs const*>(&ex);

		if(ptr == nullptr)
			return false;
		else
			return true;
	}

	static void* MarshallException(std::exception const& ex)
	{
		auto ptr = dynamic_cast<ExceptionArgs const*>(&ex);
		auto* newEx = new ExceptionArgs(*ptr);
		return newEx;
	}

	static void HandleException(void* data, void* ex)
	{
		auto targetEx = static_cast<ExceptionArgs*>(ex);
		auto catchOperand = static_cast<CatchOperand<TLambda>*>(data);

		catchOperand->lambdaStorage(*targetEx);

		delete targetEx;
	}
};

struct CatchBuilder
{
	template<typename TLambda>
	auto operator*(TLambda&& catchHandler)
		-> CatchOperand<TLambda>
	{
		return { std::move(catchHandler) };
	}
};

/*
template<typename TLambda, typename TNextLambda>
auto operator+(CatchOperand<TLambda>&& catchOperand, CatchOperand<TNextLambda>&& nextCatchOperand)
	-> std::tuple<CatchOperand<TLambda>, CatchOperand<TNextLambda>>
{
	return { std::move(catchOperand), std::move(nextCatchOperand) };
}
*/

/*
template<typename TLambda, typename... TCatchList>
auto operator+(std::tuple<TCatchList...>&& list, CatchOperand<TLambda>&& nextCatchOperand)
	-> std::tuple<TCatchList..., CatchOperand<TLambda>>
{
	return std::tuple_cat(list, std::tuple<CatchOperand<TLambda>>(std::move(nextCatchOperand)));
}
*/

template<typename TLambdaComplete, typename... TCatchList, typename TCatchLambda>
auto operator+(AsyncCompleteOperand<TLambdaComplete, std::tuple<TCatchList...>>&& operand, CatchOperand<TCatchLambda>&& nextCatchOperand)
	-> AsyncCompleteOperand<TLambdaComplete, std::tuple<TCatchList..., CatchOperand<TCatchLambda>>>
{
	dlog_print(DLOG_DEBUG, "TFC-Debug", "Pack catch lambda");
	return { std::move(operand.completeLambda), std::tuple_cat<std::tuple<TCatchList...>, std::tuple<CatchOperand<TCatchLambda>>>(std::move(operand.catchList), std::tuple<CatchOperand<TCatchLambda>>(std::move(nextCatchOperand))) };
}

template<typename TCatchOperand, typename... TTails>
struct CatchHandlerLoop<TCatchOperand, TTails...>
{
	static CatchInvoker Loop(std::exception const& ex, TCatchOperand& operand, TTails&... params)
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Enter loop", typeid(typename TCatchOperand::ExceptionArgs).name());

		bool success = TCatchOperand::CanHandle(ex);

		if(!success)
			return CatchHandlerLoop<TTails...>::Loop(ex, params...);

		return { TCatchOperand::HandleException, &operand, TCatchOperand::MarshallException(ex) };
	}
};

template<>
struct CatchHandlerLoop<>
{
	static CatchInvoker Loop(std::exception const& ex)
	{
		return {};
	}
};

template<typename... TCatchOperand>
struct CatchHandler<std::tuple<TCatchOperand...>>
{
	typedef typename Core::Metaprogramming::SequenceGenerator<sizeof...(TCatchOperand)>::Type ArgSequence;

	template<int... S>
	static CatchInvoker Func(std::exception const& ex, std::tuple<TCatchOperand...>& p, Core::Metaprogramming::Sequence<S...>)
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Expand function CatchHandler");
		return CatchHandlerLoop<TCatchOperand...>::Loop(ex, std::get<S>(p)...); //std::make_tuple(p.template Deserialize<typename std::decay<TArgs>::type>(S)...);
	}

	static CatchInvoker Func(std::exception const& ex, void* data)
	{
		return Func(ex, *static_cast<std::tuple<TCatchOperand...>*>(data), ArgSequence());
	}
};

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

#define tfc_async TFC::Core::Async::AsyncBuilder() & [=] (void* __tfc_taskHandle)
#define tfc_await TFC::Core::Async::AwaitBuilder() &
#define tfc_try_await
#define tfc_async_complete >> TFC::Core::Async::CompleteBuilder() * [=]
#define tfc_synchronize TFC::Core::Async::SynchronizeBuilder(__tfc_taskHandle) & [&] ()
#define tfc_if_abort_return

#define tfc_async_catch + TFC::Core::Async::CatchBuilder() * [=]

#endif /* ASYNC_NEW_H_ */
