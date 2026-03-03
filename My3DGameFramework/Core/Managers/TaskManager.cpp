#include "CorePch.h"
#include "TaskManager.h"
#include "Task/AsyncTask.h"

namespace MGF3D
{
    TaskManager::TaskManager() = default;
    TaskManager::~TaskManager() = default;

    void TaskManager::PushTask(Ptr<Task> task)
    {
        if (!task) return;

        MGF_LOCK_SCOPE
        {
            Lock lock(m_taskMutex);
        // 여기서 태스크에 ID를 부여하거나 관리 목록에 추가할 수 있습니다.
        }

            // AsyncTask를 통해 실제 스레드 할당 및 실행을 수행합니다.
            AsyncTask::Run(
                [task]() { task->Execute(); }
            );
    }

    void TaskManager::Update()
    {
        // 메인 스레드 동기화가 필요한 콜백들이 있다면 여기서 처리하게 됩니다.
    }
}