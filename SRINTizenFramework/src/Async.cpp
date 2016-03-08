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
#include <pthread.h>
#include <mutex>
#include <map>

namespace SRIN {

class AsyncTaskObj;

std::map<pthread_t, AsyncTaskObj*> ecoreThreadMap;

enum class TaskState
{
	Undefined = 0, Created = 1, Running = 2, Completed = 3, Cancelling = 4, Cancelled = 5
};

class AsyncTaskObj
{
public:
	Ecore_Thread* handle;
	void* retVal;
	TaskState state;
	bool dwait;
	std::function<void(void*, void*)> dwaitCaller;
	std::mutex taskMutex;
	pthread_t threadId;

	AsyncTaskObj()
	{
		handle = nullptr;
		retVal = nullptr;
		state = TaskState::Created;
		dwait = false;
		threadId = 0;
	}

	void Start()
	{
		threadId = pthread_self();
		ecoreThreadMap.insert(std::make_pair(threadId, this));
	}

	void End()
	{
		threadId = 0;
		ecoreThreadMap.erase(threadId);
	}

	~AsyncTaskObj()
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "Task destroyed %d", this);
	}

};

LIBAPI void* AwaitImpl(AsyncTaskObj* task)
{
	{
		std::lock_guard < std::mutex > lock(task->taskMutex);

		// If it was dwaited, clear dwait
		if (task->dwait)
			return nullptr;
	}

	while (task->state == TaskState::Running || task->state == TaskState::Cancelling)
		usleep(100);

	auto retVal = task->retVal;

	delete task;
	return retVal;
}

template<class R>
struct TaskContext
{
	std::function<R(void)> func;
	AsyncTaskObj* task;
	//Event<R> event;
};

template<class R>
void AsyncTaskWorker(void* data, Ecore_Thread *thread);

template<class R>
void AsyncTaskEnd(void* data, Ecore_Thread* thread);

template<>
void AsyncTaskWorker<void*>(void* data, Ecore_Thread *thread)
{
	TaskContext<void*>* context = reinterpret_cast<TaskContext<void*>*>(data);

	// Sync with other thread to tell that this thread is start processing
	{
		std::lock_guard < std::mutex > lock(context->task->taskMutex);
		context->task->state = TaskState::Running;
		context->task->Start();
	}

	auto retVal = context->func();

	{
		std::lock_guard < std::mutex > lock(context->task->taskMutex);
		context->task->retVal = retVal;
		if (context->task->state == TaskState::Cancelling)
			context->task->state = TaskState::Cancelled;
		else
			context->task->state = TaskState::Completed;
	}
	context->task->End();
}

template<>
void AsyncTaskWorker<void>(void* data, Ecore_Thread *thread)
{
	TaskContext<void*>* context = reinterpret_cast<TaskContext<void*>*>(data);

	// Sync with other thread to tell that this thread is start processing
	{
		std::lock_guard < std::mutex > lock(context->task->taskMutex);
		context->task->state = TaskState::Running;
		context->task->Start();
	}

	context->func();

	// Sync with other thread before notifying that this task is completed
	{
		std::lock_guard < std::mutex > lock(context->task->taskMutex);
		if (context->task->state == TaskState::Cancelling)
			context->task->state = TaskState::Cancelled;
		else
			context->task->state = TaskState::Completed;
	}
	context->task->End();
}

template<class T>
void AsyncTaskEnd(void* data, Ecore_Thread* thread)
{
	TaskContext<T>* context = reinterpret_cast<TaskContext<T>*>(data);

	if (context->task->dwait)
	{
		context->task->dwaitCaller(context->task, context->task->retVal);
		delete context->task;
	}

	delete context;
}

template<class T>
void AsyncTaskCancel(void* data, Ecore_Thread* thread)
{
	TaskContext<T>* context = reinterpret_cast<TaskContext<T>*>(data);

	if (context->task->dwait)
	{
		delete context->task;
	}

	delete context;
}

/*
 template<>
 LIBAPI void await(AsyncTask<void>* task)
 {
 AwaitImpl(reinterpret_cast<AsyncTaskObj*>(task));
 }
 */

template<class T>
AsyncTaskObj* CreateAsyncTaskGeneric(std::function<T(void)> func, std::function<void(void*, void*)> dispatcher)
{
	auto context = new TaskContext<T>();
	context->func = func;
	auto task = new AsyncTaskObj();
	context->task = task;

	if (dispatcher)
	{
		task->dwait = true;
		task->dwaitCaller = dispatcher;
	}

	task->handle = ecore_thread_run(AsyncTaskWorker<T>, AsyncTaskEnd<T>, AsyncTaskCancel<T>, context);
	return task;
}

template<>
LIBAPI AsyncTaskObj* CreateAsyncTask(std::function<void*(void)> func, std::function<void(void*, void*)> dispatcher)
{
	return CreateAsyncTaskGeneric(func, dispatcher);
}

template<>
LIBAPI AsyncTaskObj* CreateAsyncTask(std::function<void(void)> func, std::function<void(void*, void*)> dispatcher)
{
	return CreateAsyncTaskGeneric(func, dispatcher);
}

LIBAPI void DwaitImplVal(AsyncTaskObj* task, std::function<void(void*, void*)> dispatcher)
{
	std::lock_guard < std::mutex > lock(task->taskMutex);

	// If it is already dispatched, just return
	// TODO try to implement exception later
	if (task->dwait)
		return;

	if (task->state == TaskState::Completed)
	{
		// If the task is already completed, just perform the dispatching
		dispatcher(task, task->retVal);
		delete task;
	}
	else if (task->state != TaskState::Cancelled)
	{
		// If the task is not completed, dispatch the completion notification
		// to event
		task->dwait = true;
		task->dwaitCaller = dispatcher;
	}
}

LIBAPI void dwait(AsyncTask<void>* task, Event<AsyncTask<void>*>& ev)
{
	auto dispatcher = [ev] (void* t, void* r)
	{
		ev(reinterpret_cast<AsyncTask<void>*>(t), nullptr);
	};

	DwaitImplVal(reinterpret_cast<AsyncTaskObj*>(task), dispatcher);
}

template<>
LIBAPI AsyncTask<void>* AsyncCall<void>(std::function<void(void)> func, std::function<void(void*, void*)> dispatcher)
{
	return reinterpret_cast<AsyncTask<void>*>(CreateAsyncTask(std::function<void(void)>([func] () -> void
	{
		func();
	}), dispatcher));
}

LIBAPI void AbortImpl(AsyncTaskObj* task)
{
	{
		std::lock_guard < std::mutex > lock(task->taskMutex);
		// If it was dwaited, cancel
		task->dwait = false;
	}

	ecore_thread_cancel(task->handle);

	AwaitImpl(task);
}

LIBAPI std::function<void(void*, void*)> GetDispatcher(Event<AsyncTask<void> *>& ev)
{
	return [ev] (void* t, void* r)
	{
		ev(reinterpret_cast<AsyncTask<void>*>(t), nullptr);
	};
}

LIBAPI bool IsAborting()
{
	auto task = ecoreThreadMap[pthread_self()];

	if (task)
	{
		auto cancelling = ecore_thread_check(task->handle);
		if (cancelling)
		{
			std::lock_guard < std::mutex > lock(task->taskMutex);
			task->state = TaskState::Cancelling;
		}

		return cancelling;
	}

	return false;
}

}
