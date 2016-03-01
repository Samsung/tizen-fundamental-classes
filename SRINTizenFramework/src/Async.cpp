
/*
 * AsyncTask.cpp
 *
 *  Created on: Mar 1, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */
#include "SRIN/Core.h"
#include "SRIN/Async.h"

#include <Elementary.h>
#include <cstdlib>
#include <unistd.h>

class AsyncTask
{
public:
	void* handle;
	void* retVal;
	bool completed;

	AsyncTask()
	{
		handle = nullptr;
		retVal = nullptr;
		completed = false;
	}
};

LIBAPI void* AwaitImpl(AsyncTask* task)
{
	while(!task->completed)
		usleep(100);

	auto retVal = task->retVal;

	delete task;
	return retVal;
}

template<class R>
struct TaskContext {
	std::function<R(void)> func;
	AsyncTask* task;
	//Event<R> event;
};

template<class R>
void AsyncTaskWorker(void* data, Ecore_Thread *thread);

template<>
void AsyncTaskWorker<void*>(void* data, Ecore_Thread *thread)
{
	TaskContext<void*>* context = reinterpret_cast<TaskContext<void*>*>(data);
	context->task->retVal = context->func();
	context->task->completed = true;
	delete context;
}

template<>
void AsyncTaskWorker<void>(void* data, Ecore_Thread *thread)
{
	TaskContext<void*>* context = reinterpret_cast<TaskContext<void*>*>(data);
	context->func();
	context->task->completed = true;
	delete context;
}

template<>
LIBAPI void await(AsyncTask* task)
{
	AwaitImpl(task);
}

template<>
LIBAPI AsyncTask* CreateAsyncTask(std::function<void*(void)> func)
{
	auto context = new TaskContext<void*>();
	context->func = func;
	auto task = new AsyncTask();
	context->task = task;
	task->handle = ecore_thread_run(AsyncTaskWorker<void*>, nullptr, nullptr, context);
	return task;
}

template<>
LIBAPI AsyncTask* CreateAsyncTask(std::function<void(void)> func)
{
	auto context = new TaskContext<void>();
	context->func = func;
	auto task = new AsyncTask();
	context->task = task;
	task->handle = ecore_thread_run(AsyncTaskWorker<void>, nullptr, nullptr, context);
	return task;
}
