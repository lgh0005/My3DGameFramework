#pragma once
#include "Thread/TaskQueue.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Worker)

	class ThreadManager
	{
		MGF_DECLARE_SINGLE(ThreadManager)

	private:
		ThreadManager();
		~ThreadManager();

	public:
		void Init(uint32 cpuCount);
		void Shutdown();
		void PushTask(Task&& task);

	private:
		TaskQueue m_taskQueue;
		Vector<WorkerUPtr> m_workers;
	};
}