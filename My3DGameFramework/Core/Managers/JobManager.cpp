#include "CorePch.h"
#include "JobManager.h"
#include "TaskManager.h"
#include "Thread/MGFJob.h"

namespace MGF3D
{
    JobManager::JobManager() = default;
    JobManager::~JobManager() = default;

    bool JobManager::Init()
    {
        // 하드웨어 논리적 프로세서 개수 만큼 워커 스레드 생성
        uint32 logicalThreads = Thread::hardware_concurrency();
        if (logicalThreads == 0) logicalThreads = 4;

        // 메인 스레드 및 일부 외부 라이브러리를 위한 공간 제외 나머지를 워커로 잡아둠
        uint32 workerCount = CommonUtils::Select<uint32>((logicalThreads > 1), logicalThreads - 1, 1);
        m_workers.Reserve(workerCount);

        // 1. 워커 스레드 생성
        for (uint32 i = 0; i < workerCount; ++i)
        {
            if (!CreateWorker())
            {
                MGF_LOG_ERROR("JobManager: Failed to prepare worker {0}", i);
                return false;
            }
        }

        // 2. 워커 스레드 실행
        auto threadNamePrefix = SString("MGF_Worker_");
        for (uint32 i = 0; i < workerCount; ++i)
        {
            SString workerName = threadNamePrefix + SString(StringUtils::ToString(i));

            auto workerLoop = [this, i]() { this->WorkerLoop(m_workers[i].Get()); };

            if (!m_workers[i]->Launch(workerName, workerLoop))
            {
                MGF_LOG_ERROR("JobManager: Failed to launch worker {0}", i);
                return false;
            }

            m_workers[i]->WaitUntilReady();
        }

        MGF_LOG_INFO("JobManager: Successfully initialized with {0} workers.", workerCount);
        return true;
    }

    bool JobManager::CreateWorker()
    {
        auto worker = MakeUnique<MGFJob>();
        if (!worker) return false;

        m_workers.PushBack(std::move(worker));
        return true;
    }

    void JobManager::WorkerLoop(MGFJob* self)
    {
        self->NotifyReady();

        while (!m_isShutdown.load())
        {
            auto task = MGF_TASK.PopTask();
            
            if (task) task->Execute();
            else MGF_TASK.WaitForTask();
        }
    }

    void JobManager::Shutdown()
    {
        if (m_isShutdown) return;

        m_isShutdown = true;

        // 잠자고 있는 모든 워커들을 강제로 깨워 루프를 탈출하게 함
        MGF_TASK.Broadcast();

        // MGFJob의 소멸자가 호출되면서 내부의 MGFThread가 Join/정리됨
        for (auto& worker : m_workers)
        {
            if (worker) 
                worker->Join();
        }
        m_workers.Release();

        // m_workers.Release(); // INFO : 현재 여기에서 is_block_type_valid 오류를 일으키고 있음

        MGF_LOG_INFO("JobManager: All workers have been shut down.");
    }
}