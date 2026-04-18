#pragma once
#include "Thread/TaskQueue.h"

namespace MGF3D
{
	MGF_CLASS_PTR(CPUWorker)
	MGF_CLASS_PTR(GPUWorker)

	class ThreadManager
	{
		MGF_DECLARE_SINGLE(ThreadManager)

	private:
		ThreadManager();
		~ThreadManager();

	public:
		void Init(uint32 cpuCount, uint32 gpuCount);
		void Shutdown();

	public:
		void PushCPUTask(Task&& task);
		void PushGPUTask(Task&& task);

	private:
		TaskQueue m_cpuQueue;
		TaskQueue m_gpuQueue;

		Vector<CPUWorkerUPtr> m_cpuWorkers;
		Vector<GPUWorkerUPtr> m_gpuWorkers;
	};
}