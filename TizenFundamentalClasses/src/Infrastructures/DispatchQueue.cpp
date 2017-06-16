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

using namespace TFC;
using namespace TFC::Infrastructures;

struct DispatchQueue::Implementation
{
	std::deque<std::function<void()>> functionsToRun;
	std::thread dispatchThread;
	std::mutex sharedMutex;
	std::condition_variable waitFlag;
	bool exit { false };
	void MessageLoop();
};


DispatchQueue::DispatchQueue() : impl(new DispatchQueue::Implementation)
{
}

DispatchQueue::~DispatchQueue()
{
	delete impl;
}

void DispatchQueue::Start()
{
	std::thread newThread { [i = impl] { i->MessageLoop(); } };
	impl->dispatchThread = std::move(newThread);
}

void DispatchQueue::Stop()
{
	this->impl->exit = true;
	this->impl->dispatchThread.join();
}

void DispatchQueue::Dispatch(std::function<void()>&& task)
{
	std::unique_lock<std::mutex> lock(impl->sharedMutex);
	impl->functionsToRun.push_back(std::move(task));
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
		runThis();
	}
}
