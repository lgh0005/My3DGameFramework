#include "CorePch.h"
#include "ThreadManager.h"
#include "Thread/CPUWorker.h"
#include "Thread/GPUWorker.h"
#include "Window/MGFWindow.h"
#include "Managers/WindowManager.h"

namespace MGF3D
{
	ThreadManager::ThreadManager() = default;
	ThreadManager::~ThreadManager() = default;

	void ThreadManager::Init(uint32 cpuCount, uint32 gpuCount)
	{
		// 1. CPU 워커 고용 및 시작
		for (uint32 i = 0; i < cpuCount; ++i)
		{
			auto worker = MakeUnique<CPUWorker>(m_cpuQueue);
			worker->Start();
			m_cpuWorkers.push_back(std::move(worker));
		}

		// 2. GPU 워커 고용 및 시작
		for (uint32 i = 0; i < gpuCount; ++i)
		{
			MGFWindowUPtr sharedContextWindow = MGF_WINDOW.CreateSharedContextWindow();
			auto worker = MakeUnique<GPUWorker>(m_gpuQueue, std::move(sharedContextWindow));
			worker->Start();
			m_gpuWorkers.push_back(std::move(worker));
		}

		MGF_LOG_WARN("ThreadManager: Successfully initialized {} CPU Workers and {} GPU Workers.", cpuCount, gpuCount);
	}

	void ThreadManager::Shutdown()
	{
		// 1. 영업 종료 선언 (모든 큐 정지 및 대기 중인 스레드 기상)
		m_cpuQueue.Stop();
		m_gpuQueue.Stop();

		// 2. 스레드 소멸 (소멸자에서 Join 호출됨)
		m_cpuWorkers.clear();
		m_gpuWorkers.clear();
	}

	void ThreadManager::PushCPUTask(Task&& task)
	{
		m_cpuQueue.Push(std::move(task));
	}

	void ThreadManager::PushGPUTask(Task&& task)
	{
		m_gpuQueue.Push(std::move(task));
	}
}