#include "ConverterPch.h"
#include "ThreadManager.h"
#include "Thread/Worker.h"

namespace MGF3D
{
	ThreadManager::ThreadManager() = default;
	ThreadManager::~ThreadManager() = default;

	void ThreadManager::Init(uint32 cpuCount)
	{
		for (uint32 i = 0; i < cpuCount; ++i)
		{
			auto worker = MakeUnique<Worker>(m_taskQueue);
			worker->Start();
			m_workers.push_back(std::move(worker));
		}
	}

	void ThreadManager::Shutdown()
	{
		m_taskQueue.Stop();
		m_workers.clear();
	}

	void ThreadManager::PushTask(Task&& task)
	{
		m_taskQueue.Push(std::move(task));
	}
}