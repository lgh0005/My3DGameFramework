#include "CorePch.h"
#include "CPUWorker.h"

namespace MGF3D
{
	CPUWorker::CPUWorker(TaskQueue& queue) : Super(queue) { }
	CPUWorker::~CPUWorker() = default;

	void CPUWorker::Run()
	{
		Task task;
		while (m_queue.WaitAndPop(task))
		{
			if (task) 
				task();
		}
	}
}