/*
 * Async.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: Gilang M. Hamidy
 */


#include "TFC/Async.h"

#include <Elementary.h>
#include <mutex>
#include <iostream>
#include <map>

namespace {

using namespace TFC::Core::Async;


struct AsyncContext
{
	AsyncHandlerPayload payload;
	std::mutex contextLock;
	std::mutex runningLock;
	Ecore_Thread* threadHandle;
	bool running;
	bool exceptionOccured;
	CatchInvoker catchInvoker;

	std::type_info const* exceptionType;
	std::string exceptionMessage;
	std::mutex exceptionLock;
	bool exceptionTransfered;

	AsyncContext()
	{
		running = false;
		threadHandle = nullptr;
		exceptionOccured = false;
		table[this] = this;
		exceptionType = nullptr;
		exceptionTransfered = false;
	}

	~AsyncContext()
	{
		std::cout << "Destructor AsyncContext\n";
		table.erase(this);
	}

	static std::map<AsyncContext*, AsyncContext*> table;

	static AsyncContext* TryGet(void* handle)
	{
		auto ret = table.find(reinterpret_cast<AsyncContext*>(handle));

		if(ret == table.end())
			return nullptr;
		else
			return ret->second;
	}
};

struct SynchronizeContext
{
	std::mutex syncMutex;
	SynchronizeHandlerPayload* handlerPayload;
};

std::map<AsyncContext*, AsyncContext*> AsyncContext::table;

void Async_Thread(void* data, Ecore_Thread* thd)
{
	auto ctx = reinterpret_cast<AsyncContext*>(data);

	dlog_print(DLOG_DEBUG, LOG_TAG, "In thread. Ctx: %d", ctx);

	// Acquire running lock

	{
		// Update context
		std::lock_guard<std::mutex> lock(ctx->contextLock);
		ctx->running = true;
	}

	//std::cout << "Thread Start: " << (int)ctx << std::endl;

	try
	{
		// Run the task
		auto taskFunc = ctx->payload.taskFunc;
		taskFunc(ctx->payload.internalData, ctx);
	}
	catch(std::exception const& c)
	{
		// Update context due to exception
		{
			std::lock_guard<std::mutex> lock(ctx->contextLock);
			ctx->exceptionOccured = true;
		}

		dlog_print(DLOG_DEBUG, "TFC-Debug", "Handle exception");
		if(ctx->payload.catchHandlerData != nullptr)
		{

			dlog_print(DLOG_DEBUG, "TFC-Debug", "Handle exception catch find");
			auto catchHandler = ctx->payload.catchHandler;
			ctx->catchInvoker = catchHandler(c, ctx->payload.catchHandlerData);

			if(!ctx->catchInvoker.handled)
				throw;
		}
		else if(ctx->payload.awaitable)
		{
			// Store the exception
			auto msg = c.what();

			if(msg != nullptr)
				ctx->exceptionMessage = msg;

			ctx->exceptionType = &typeid(c);
		}
		else throw;
	}

	//std::cout << "Thread Completed: " << (int)ctx << std::endl;

	{
		// Update context
		std::lock_guard<std::mutex> lock(ctx->contextLock);
		ctx->running = false;
	}

	ctx->runningLock.unlock();
}

void Async_Cancel(void* data, Ecore_Thread* thd)
{
	auto ctx = reinterpret_cast<AsyncContext*>(data);

	auto finalizeFunc = ctx->payload.finalizeFunc;
	finalizeFunc(ctx->payload.internalData);

	delete ctx;
}

void Async_Complete(void* data, Ecore_Thread* thd)
{
	auto ptrCasted = reinterpret_cast<AsyncContext*>(data);

	if(!ptrCasted->payload.awaitable)
	{
		std::unique_ptr<AsyncContext> ctx(ptrCasted);

		if(!ctx->exceptionOccured)
		{
			// If it is not awaitable, notify the completion function
			auto completeInvoker = ctx->payload.completeInvoker;
			completeInvoker(ctx->payload.internalData);

			// Then finalize their internals
			auto finalizeFunc = ctx->payload.finalizeFunc;
			finalizeFunc(ctx->payload.internalData);
		}
		else
		{
			ctx->catchInvoker.InvokeHandler();

			if(ctx->payload.catchHandlerData != nullptr)
			{
				auto finalizeFunc = ctx->payload.catchHandlerFinalizeFunc;
				finalizeFunc(ctx->payload.catchHandlerData);
			}
		}
	}
}

void Async_Notify(void* data, Ecore_Thread* thd, void* notifData)
{
	//auto ctx = reinterpret_cast<AsyncContext*>(data);
	auto syncCtx = reinterpret_cast<SynchronizeContext*>(notifData);
	// Run the function
	syncCtx->handlerPayload->lambdaInvokerFunc(syncCtx->handlerPayload->packagePtr);
	// Release lock
	syncCtx->syncMutex.unlock();
}

}

LIBAPI
void* TFC::Core::Async::RunAsyncTask(AsyncHandlerPayload payload)
{


	AsyncContext* ctx = new AsyncContext;
	ctx->payload = payload;
	ctx->runningLock.lock();

	dlog_print(DLOG_DEBUG, LOG_TAG, "Async task run. Ctx: %d", ctx);

	ctx->threadHandle = ecore_thread_feedback_run(Async_Thread,
												  Async_Notify,
												  Async_Complete,
												  Async_Cancel,
												  ctx,
												  EINA_FALSE);


	return ctx;
}

void ThrowHelper(std::type_info const* exceptionType, std::string const& message)
{
	// Currently throw TFCException
	throw TFC::TFCException(message);
}

LIBAPI
void TFC::Core::Async::AwaitAsyncTask(void* handle, void*& package, bool& doFinalize)
{
	auto ctx = AsyncContext::TryGet(handle);

	if(ctx == nullptr)
	{
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Something wrong...");
		return;
	}
	else
	{
		ctx->runningLock.lock(); // Wait until the thread is completed

		if(!ctx->exceptionOccured)
		{
			package = ctx->payload.internalData;

			if(ctx->payload.awaitable)
			{
				doFinalize = true;
				delete ctx;
			}
		}
		else
		{
			std::unique_ptr<AsyncContext> ctxPtr;

			if(ctx->payload.awaitable)
				ctxPtr = std::unique_ptr<AsyncContext>(ctx);

			ThrowHelper(ctx->exceptionType, ctx->exceptionMessage);
		}
	}
}

#include<sstream>

LIBAPI
void TFC::Core::Async::SynchronizeCall(void* taskHandle, SynchronizeHandlerPayload* p)
{
	auto ctx = AsyncContext::TryGet(taskHandle);

	if(ctx == nullptr)
	{
		std::stringstream s;
		s << "Invalid task handle received on SynchronizeCall: " << (int32_t)taskHandle;
		throw TFCException(s.str());
	}

	else
	{
		SynchronizeContext syncCtx;
		syncCtx.syncMutex.lock();
		syncCtx.handlerPayload = p;
		ecore_thread_feedback(ctx->threadHandle, &syncCtx);
		// Wait until the lock is released (a.k.a. Async_Notify is runned)
		syncCtx.syncMutex.lock();
		delete p;
	}

}
