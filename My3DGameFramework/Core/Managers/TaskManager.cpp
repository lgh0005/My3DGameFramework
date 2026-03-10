#include "CorePch.h"
#include "TaskManager.h"
#include "JobManager.h"

namespace MGF3D
{
    TaskManager::TaskManager() = default;
    TaskManager::~TaskManager() = default;

    void TaskManager::Shutdown()
    {
        // 1. 이미 JobManager에서 스레드 정리가 끝났겠지만, 
        // 여기서 한 번 더 브로드캐스트를 날려 확실히 잠을 깨워줍니다.
        Broadcast();

        // 2. 남은 태스크들을 명시적으로 비웁니다. 
        { MGF_LOCK_SCOPE(m_queueMutex); m_taskQueue.Release(); }
        { MGF_LOCK_SCOPE(m_mainMutex); m_mainQueue.Release(); }
    }

    TaskUPtr TaskManager::AcquireTask(Action<> work, Action<> onComplete)
    {
        return m_taskPool.Acquire(std::move(work), std::move(onComplete));
    }

    void TaskManager::PushMainTask(TaskUPtr task)
    {
        if (!task) return;

        MGF_LOCK_SCOPE(m_mainMutex);
        m_mainQueue.PushBack(std::move(task));
    }

    void TaskManager::PushTask(TaskUPtr task)
    {
        if (!task) return;

        {
            MGF_LOCK_SCOPE(m_queueMutex);
            m_taskQueue.PushBack(std::move(task));
        }

        // 대기 중인 워커 중 하나를 깨워 일을 시킵니다.
        m_condition.notify_one();
    }

    TaskUPtr TaskManager::PopTask()
    {
        MGF_LOCK_SCOPE(m_queueMutex);

        if (m_taskQueue.Empty())
            return nullptr;

        return m_taskQueue.PopFront();
    }

    void TaskManager::WaitForTask()
    {
        UniqueLock lock(m_queueMutex);

        // 큐에 일이 있거나, 엔진이 종료 중이면 깨어납니다.
        m_condition.wait(lock, [this] { return !m_taskQueue.Empty() || JobManager::Instance().IsShutdown(); });
    }

    void TaskManager::Broadcast()
    {
        // 모든 워커를 깨워 Shutdown 여부를 확인하게 합니다.
        m_condition.notify_all();
    }

    void TaskManager::Update()
    {
        // 메인 스레드 큐에 쌓인 일감들을 로컬로 옮겨와 처리합니다.
        // 락 점유 시간을 최소화하기 위함입니다.
        LDeque<TaskUPtr> localQueue;

        // 메인 큐에서 일감을 로컬로 옮겨오는 찰나에만 락을 겁니다.
        {
            MGF_LOCK_SCOPE(m_mainMutex);
            if (m_mainQueue.Empty()) return;
            localQueue = std::move(m_mainQueue);
        }

        while (!localQueue.Empty())
        {
            auto task = localQueue.PopFront();
            if (task) task->Execute();
        }
    }
}