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


DispatchQueue::DispatchQueue() : impl(new DispatchQueue::Implementation)
{
}

DispatchQueue::~DispatchQueue()
{
	Stop();
	delete impl;
}

void DispatchQueue::Start()
{
	pthread_create(&impl->threadInfo, NULL, (Implementation::NativeThreadProc), impl);
	return;
}

void DispatchQueue::Stop()
{
	if(impl->threadInfo)
	{
		this->impl->exit = true;
		impl->waitFlag.notify_one();
		pthread_join(this->impl->threadInfo, nullptr);
	}
}

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
