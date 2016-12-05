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

	AsyncContext()
	{
		running = false;
		threadHandle = nullptr;
		table[this] = this;
	}

	~AsyncContext()
	{
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

std::map<AsyncContext*, AsyncContext*> AsyncContext::table;

void Async_Thread(void* data, Ecore_Thread* thd)
{
	auto ctx = reinterpret_cast<AsyncContext*>(data);

	// Acquire running lock

	{
		// Update context
		std::lock_guard<std::mutex> lock(ctx->contextLock);
		ctx->running = true;
	}

	//std::cout << "Thread Start: " << (int)ctx << std::endl;

	// Run the task
	auto taskFunc = ctx->payload.taskFunc;
	taskFunc(ctx->payload.internalData);

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
	auto ctx = reinterpret_cast<AsyncContext*>(data);

	if(!ctx->payload.awaitable)
	{
		// If it is not awaitable, notify the completion function
		auto completeInvoker = ctx->payload.completeInvoker;
		completeInvoker(ctx->payload.internalData);

		// Then finalize their internals
		auto finalizeFunc = ctx->payload.finalizeFunc;
		finalizeFunc(ctx->payload.internalData);
	}

	// Clean up tast context
	delete ctx;
}

void Async_Notify(void* data, Ecore_Thread* thd, void* notifData)
{
	auto ctx = reinterpret_cast<AsyncContext*>(data);
}

}

LIBAPI
void* TFC::Core::Async::RunAsyncTask(AsyncHandlerPayload payload)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Async task run");

	AsyncContext* ctx = new AsyncContext;
	ctx->payload = payload;
	ctx->runningLock.lock();

	ctx->threadHandle = ecore_thread_feedback_run(Async_Thread,
												  Async_Notify,
												  Async_Complete,
												  Async_Cancel,
												  ctx,
												  EINA_FALSE);


	return ctx;
}

LIBAPI
void TFC::Core::Async::AwaitAsyncTask(void* handle, void*& package, bool& doFinalize)
{
	auto ctx = AsyncContext::TryGet(handle);

	if(ctx == nullptr)
		return;
	else
	{
		ctx->runningLock.lock(); // Wait until the thread is completed
		package = ctx->payload.internalData;

		if(ctx->payload.awaitable)
		{
			doFinalize = true;
		}
	}
}
