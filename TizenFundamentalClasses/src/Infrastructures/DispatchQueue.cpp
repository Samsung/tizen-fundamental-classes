/*
 * DispatchQueue.cpp
 *
 *  Created on: Jun 16, 2017
 *      Author: gmh
 */

#include "TFC/Infrastructures/DispatchQueue.h"

#include <thread>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <dlog.h>
#include <pthread.h>
#include <utility>
#include <queue>

using namespace TFC;
using namespace TFC::Infrastructures;

struct DispatchQueue::Implementation
{
	std::deque<std::function<void()>> functionsToRun;
	std::mutex sharedMutex;
	std::condition_variable waitFlag;
	bool exit { false };
	void MessageLoop();
	pthread_t threadInfo = 0;

	static void* NativeThreadProc(void* data)
	{
		static_cast<Implementation*>(data)->MessageLoop();
		return nullptr;
	}
};

TFC::Infrastructures::
DispatchQueue::DispatchQueue() : impl(new DispatchQueue::Implementation)
{
}

TFC::Infrastructures::
DispatchQueue::~DispatchQueue()
{
	Stop();
	delete impl;
}

LIBAPI
void DispatchQueue::Start()
{
	this->impl->exit = false;
	pthread_create(&impl->threadInfo, NULL, (Implementation::NativeThreadProc), impl);
	return;
}

LIBAPI
void DispatchQueue::Stop()
{
	if(impl->threadInfo)
	{
		this->impl->exit = true;
		impl->waitFlag.notify_one();
		pthread_join(this->impl->threadInfo, nullptr);
	}
}

LIBAPI
void DispatchQueue::Dispatch(std::function<void()>&& task)
{
	{
		std::unique_lock<std::mutex> lock(impl->sharedMutex);
		impl->functionsToRun.push_back(std::move(task));
	}
	impl->waitFlag.notify_one();
}

void DispatchQueue::Implementation::MessageLoop()
{
	while(true)
	{
		std::function<void()> runThis;

		{
			std::unique_lock<std::mutex> lock(sharedMutex);
			waitFlag.wait(lock, [this] { return !this->functionsToRun.empty() || this->exit; });
			if(exit)
				break;

			// Move the functions out
			runThis = std::move(functionsToRun.front());
			functionsToRun.pop_front();
		}

		// Perform function
		if(runThis)
			runThis();
	}
}

struct PrioritizedDispatchQueue::Implementation
{
	typedef std::pair<int64_t, std::function<void()>> FuncPair;

	struct Compare
	{
		auto operator()(FuncPair const& a, FuncPair const& b) {
			return a.first < b.first;
		};
	};


	std::priority_queue<FuncPair, std::deque<FuncPair>, Compare> functionsToRun;
	std::mutex sharedMutex;
	std::condition_variable waitFlag;

	bool exit { false };
	void MessageLoop();
	pthread_t threadInfo = 0;

	static void* NativeThreadProc(void* data)
	{
		static_cast<Implementation*>(data)->MessageLoop();
		return nullptr;
	}
};

void PrioritizedDispatchQueue::Implementation::MessageLoop()
{
	while(true)
	{
		std::function<void()> runThis;

		{
			std::unique_lock<std::mutex> lock(sharedMutex);
			waitFlag.wait(lock, [this] { return !this->functionsToRun.empty() || this->exit; });
			if(exit)
				break;

			// Move the functions out
			runThis = std::move(functionsToRun.top().second);
			functionsToRun.pop();
		}

		// Perform function
		if(runThis)
			runThis();
	}
}


LIBAPI
PrioritizedDispatchQueue::PrioritizedDispatchQueue() : impl(new PrioritizedDispatchQueue::Implementation)
{
}

LIBAPI
PrioritizedDispatchQueue::~PrioritizedDispatchQueue()
{
	Stop();
	delete impl;
}

LIBAPI
void PrioritizedDispatchQueue::Start()
{

	this->impl->exit = false;
	pthread_create(&impl->threadInfo, NULL, (Implementation::NativeThreadProc), impl);
	return;
}

LIBAPI
void PrioritizedDispatchQueue::Stop()
{
	if(impl->threadInfo)
	{
		this->impl->exit = true;
		impl->waitFlag.notify_one();
		pthread_join(this->impl->threadInfo, nullptr);
	}
}

LIBAPI
void PrioritizedDispatchQueue::Dispatch(int64_t priority, std::function<void()>&& task)
{
	{
		std::unique_lock<std::mutex> lock(impl->sharedMutex);
		impl->functionsToRun.push({priority, std::move(task)});
	}
	impl->waitFlag.notify_one();
}

