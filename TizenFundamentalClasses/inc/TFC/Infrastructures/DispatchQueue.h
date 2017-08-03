/*
 * DispatchQueue.h
 *
 *  Created on: Jun 16, 2017
 *      Author: gmh
 */

#ifndef TFC_INFRASTRUCTURES_DISPATCHQUEUE_H_
#define TFC_INFRASTRUCTURES_DISPATCHQUEUE_H_


#include "TFC/Core.h"
#include <functional>

namespace TFC {
namespace Infrastructures {

class LIBAPI DispatchQueue
{
public:
	DispatchQueue();
	~DispatchQueue();
	void Start();
	void Stop();
	void Dispatch(std::function<void()>&& task);
private:
	struct Implementation;
	Implementation* impl;
};

class LIBAPI PrioritizedDispatchQueue
{
public:
	PrioritizedDispatchQueue();
	~PrioritizedDispatchQueue();
	void Start();
	void Stop();
	void Dispatch(int64_t priority, std::function<void()>&& task);
private:
	struct Implementation;
	Implementation* impl;
};

}
}



#endif /* TFC_INFRASTRUCTURES_DISPATCHQUEUE_H_ */
