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
#include <utility>
#include <dlog.h>
#include "TFC/Core.h"
#include "TFC/Core/Introspect.h"

namespace TFC {

template<typename TReturnValue>
struct Async;


namespace Core {
namespace Async {

template<typename TReturnValue>
struct AsyncTask;

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


template<
	typename TLambda,
	typename TEventType = void,
	typename TReturnValue = typename Introspect::CallableObject<TLambda>::ReturnType
>
struct AsyncOperand
{
	typedef TReturnValue											  ReturnType;
	typedef AsyncTask<TReturnValue>									  AsyncTaskType;
	typedef SharedEventObject<AsyncTask<TReturnValue>*, TReturnValue> EventType;
	TLambda&& asyncFunc;

	AsyncOperand(TLambda&& aLambda) :
		asyncFunc(std::move(aLambda))
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Build async operand");
	}
};



template<typename TLambda>
struct AsyncOperand<TLambda, void, void>
{
	typedef void											ReturnType;
	typedef AsyncTask<void>									AsyncTaskType;
	typedef SharedEventObject<AsyncTask<void>*, void*> 		EventType;

	TLambda&& asyncFunc;

	AsyncOperand(TLambda&& aLambda) :
		asyncFunc(std::move(aLambda))
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Build async operand");
	}
};

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

template<typename TLambda,
		 typename TIntrospect = Introspect::CallableObject<TLambda>,
		 bool TNonVoidParam   = TIntrospect::Arity == 1,
		 bool TParamOneOrZero = TIntrospect::Arity <= 1>
struct AsyncCompleteOperand
{
	typedef typename Introspect::CallableObject<TLambda>::template Args<0> ArgumentType;
	static constexpr bool Valid = true;
	static constexpr bool IsVoid = false;

	TLambda&& CompleteLambda;

	AsyncCompleteOperand(TLambda&& CompleteLambda) :
			CompleteLambda(std::move(CompleteLambda))
	{

	}

	static AsyncCompleteOperand<TLambda> MakeOperand(TLambda&& lambda)
	{
		return { std::move(lambda) };
	}

	template<typename TLambdaAsync,
			 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
			 bool 	  RuleValidation   = std::is_same<typename TIntrospect::template Args<0>,
								  	  	  	  	  	  typename TIntrospectAsync::ReturnType>::value>
	struct Match
	{
		static constexpr bool Valid = RuleValidation;
	};
};

template<typename TLambda, typename TIntrospect>
struct AsyncCompleteOperand<TLambda, TIntrospect, false, true>
{
	static constexpr bool Valid = true;
	static constexpr bool IsVoid = true;

	TLambda&& CompleteLambda;

	AsyncCompleteOperand(TLambda&& CompleteLambda) :
				CompleteLambda(std::forward<TLambda>(CompleteLambda))
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

template<typename TLambda, typename TIntrospect>
struct AsyncCompleteOperand<TLambda, TIntrospect, false, false>
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

template<typename TLambda, typename TReturnValue, typename TLambdaComplete>
struct AsyncOperand<TLambda, AsyncCompleteOperand<TLambdaComplete>, TReturnValue> :
	AsyncOperand<TLambda, void, TReturnValue>
{
	TLambdaComplete&& lambdaComplete;

	AsyncOperand(TLambda&& func, TLambdaComplete&& lambdaComplete) :
		AsyncOperand<TLambda, void, TReturnValue>(std::forward<TLambda>(func)),
		lambdaComplete(std::forward<TLambdaComplete>(lambdaComplete))
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Async operand with lambda complete constructor");
	}
};

template<typename T>
struct AsyncResult
{
	typedef T ReturnType;
	T value;

	AsyncResult(T&& value) : value(std::move(value)) { };
};

template<>
struct AsyncResult<void>
{

};

template<typename TLambda, typename TReturnType = typename Introspect::CallableObject<TLambda>::ReturnType>
struct CompleteLambdaInvoker;


template<typename TLambda, typename TReturnType = typename Introspect::CallableObject<TLambda>::ReturnType>
struct CompleteEventInvoker;

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

template<typename TReturnType>
struct AsyncCompletePackage
{
	template<typename TLambdaComplete>
	struct LambdaStorage
	{
		TLambdaComplete lambdaStorage;

		explicit LambdaStorage(TLambdaComplete&& l) :
			lambdaStorage(std::move(l))
		{

		}

		static void Invoker(void* storage, TReturnType&& val)
		{
			auto thiz = reinterpret_cast<LambdaStorage<TLambdaComplete>*>(storage);
			thiz->lambdaStorage(std::move(val));
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
	void (*invokerFunc)(void*, TReturnType&& val);
	void (*deleterFunc)(void*);

	void operator()(TReturnType&& val)
	{
		if(storage != nullptr)
			invokerFunc(storage, std::move(val));
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

	SharedEventObject<AsyncTask<ReturnType>*, typename AsyncTypeSelector<ReturnType>::EventDataType> eventComplete;
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

	}

	AsyncPackage(
			TLambda&& functionAsync,
			SharedEventObject<AsyncTask<ReturnType>*, typename AsyncTypeSelector<ReturnType>::EventDataType> eventComplete) :
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
	static void Func(void* package)
	{
		auto ptr = reinterpret_cast<AsyncPackage<TLambda>*>(package);
		ptr->result = new AsyncResult<TReturnType>(ptr->functionAsync());
	}
};

template<typename TLambda>
struct AsyncWorker<TLambda, void>
{
	static void Func(void* package)
	{
		auto ptr = reinterpret_cast<AsyncPackage<TLambda>*>(package);
		ptr->functionAsync();
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

template<typename T>
void AsyncFinalizeHandler(void* package)
{
	auto ptr = reinterpret_cast<AsyncPackage<T>*>(package);
	delete ptr;
}


template<typename TLambda, typename TEvent, typename TReturnValue>
auto PackOperand(AsyncOperand<TLambda, TEvent, TReturnValue>&& operand)
	-> typename std::enable_if<!std::is_void<TEvent>::value, AsyncPackage<TLambda>*>::type
{
	dlog_print(DLOG_DEBUG, "TFC-Debug", "Pack operand");
	/*
	AsyncPackage<TReturnValue>* ret = new AsyncPackage<TReturnValue>;
	ret->functionAsync = operand.asyncFunc;
	ret->eventComplete = operand.eventRef;
	ret->completeInvoker = CompleteEventInvoker<TReturnValue>;
	ret->awaitable = false
	*/

	return new AsyncPackage<TLambda> { std::forward<TLambda>(operand.asyncFunc), operand.eventRef };
}

template<typename TLambda, typename TLambdaComplete, typename TReturnValue>
auto PackOperand(AsyncOperand<TLambda, AsyncCompleteOperand<TLambdaComplete>, TReturnValue>&& operand)
	-> AsyncPackage<TLambda>*
{
	dlog_print(DLOG_DEBUG, "TFC-Debug", "Pack operand");
	/*
	AsyncPackage<TReturnValue>* ret = new AsyncPackage<TReturnValue>;
	ret->functionAsync = operand.asyncFunc;
	ret->lambdaComplete = std::move(operand.lambdaComplete);
	ret->completeInvoker = CompleteLambdaInvoker<TReturnValue>;
	ret->awaitable = false;
	*/
	return new AsyncPackage<TLambda> { std::forward<TLambda>(operand.asyncFunc), std::move(operand.lambdaComplete) };
}

template<typename TLambda>
auto PackOperand(AsyncOperand<TLambda, void>&& operand)
	-> AsyncPackage<TLambda>*
{
	dlog_print(DLOG_DEBUG, "TFC-Debug", "Pack operand");
	/*
	AsyncPackage<typename AsyncOperand<TLambda>::ReturnType>* ret = new AsyncPackage<typename AsyncOperand<TLambda>::ReturnType>;
	ret->functionAsync = operand.asyncFunc;
	*/

	return new AsyncPackage<TLambda> { std::forward<TLambda>(operand.asyncFunc) };
}

struct AsyncHandlerPayload
{
	typedef void(*FunctionType)(void*);
	FunctionType taskFunc;
	FunctionType completeInvoker;
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
			auto package = reinterpret_cast<AsyncTaskInterface<TReturnValue>*>(packagePtr);
			TReturnValue ret = package->result->value;
			auto deleter = reinterpret_cast<AsyncTaskInterface<void>*>(packagePtr)->deleterFunction;
			deleter(packagePtr);
			return ret;
		}
		else
		{
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

struct AsyncBuilder
{
	template<typename TLambda, typename TEvent>
	auto operator& (AsyncOperand<TLambda, TEvent>&& operand)
		-> AsyncTask<typename AsyncOperand<TLambda>::ReturnType>*
	{
		typedef typename AsyncOperand<TLambda>::ReturnType TReturnValue;

		auto packed = PackOperand(std::forward<AsyncOperand<TLambda, TEvent>>(operand));

		AsyncHandlerPayload payload = {
			AsyncWorker<TLambda>::Func,
			packed->completeInvoker,
			AsyncFinalizeHandler<TLambda>,
			packed,
			&packed->taskHandle,
			packed->awaitable
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
		 typename 			  = typename std::enable_if<TIntrospect::Arity == 0>::type>
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
		return { async, std::move(after.CompleteLambda) };
	}
is not applicable if we want to use std::enable_if and will result in [template parameter redefines default argument] error.
See : https://stackoverflow.com/questions/29502052/template-specialization-and-enable-if-problems
*/

template<typename TLambdaAsync,
		 typename TLambdaAfter,
		 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
		 typename TIntrospectAfter = Introspect::CallableObject<TLambdaAfter>,
		 typename std::enable_if<AsyncCompleteOperand<TLambdaAfter>::template Match<TLambdaAsync>::Valid, int>::type* = nullptr>
auto operator>>(TLambdaAsync&& async, AsyncCompleteOperand<TLambdaAfter> after)
	-> AsyncOperand<TLambdaAsync, AsyncCompleteOperand<TLambdaAfter>>
{
	dlog_print(DLOG_DEBUG, "TFC-Debug", "Build async with >> operator and lambda after");
	return { std::forward<TLambdaAsync>(async), std::move(after.CompleteLambda) };
}

// Assert that CompleteBuilder >> operator will receive lambda with appropriate parameter
template<typename TLambdaAsync,
		 typename TLambdaAfter,
		 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
		 typename TIntrospectAfter = Introspect::CallableObject<TLambdaAfter>,
		 typename std::enable_if<!AsyncCompleteOperand<TLambdaAfter>::Valid, int>::type* = nullptr>
auto operator>>(TLambdaAsync&& async, AsyncCompleteOperand<TLambdaAfter> after)
	-> AsyncOperand<TLambdaAsync, AsyncCompleteOperand<TLambdaAfter>>
{

	return { nullptr, nullptr };
}


template<typename TLambdaAsync,
		 typename TLambdaAfter,
		 typename TIntrospectAsync = Introspect::CallableObject<TLambdaAsync>,
		 typename TIntrospectAfter = Introspect::CallableObject<TLambdaAfter>,
		 typename std::enable_if<!AsyncCompleteOperand<TLambdaAfter>::template Match<TLambdaAsync>::Valid, int>::type* = nullptr>
auto operator>>(TLambdaAsync&& async, AsyncCompleteOperand<TLambdaAfter> after)
	-> AsyncOperand<TLambdaAsync, AsyncCompleteOperand<TLambdaAfter>>
{
	static_assert(std::is_same<typename TIntrospectAfter::template Args<0>, typename TIntrospectAsync::ReturnType>::value,
			"Parameter for tfc_async_complete lambda must match with the return value from tfc_async.");
	return { nullptr, nullptr };
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

#define tfc_async TFC::Core::Async::AsyncBuilder() & [=] ()
#define tfc_await TFC::Core::Async::AwaitBuilder() &
#define tfc_try_await
#define tfc_async_complete >> TFC::Core::Async::CompleteBuilder() * [=]
#define tfc_if_abort_return

#endif /* ASYNC_NEW_H_ */
