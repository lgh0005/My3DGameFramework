#pragma once
#include "Task/Task.h"
#include "Containers/Slab/SMap.h"

namespace MGF3D
{
    class TaskManager
    {
        MGF_DECLARE_SINGLE(TaskManager)

    private:
        TaskManager();
        ~TaskManager();

    public:
        // 새로운 태스크를 시스템에 등록하고 비동기 실행을 트리거합니다.
        void PushTask(Ptr<Task> task);

        // (추후 확장) 메인 스레드에서 실행되어야 할 콜백들을 처리합니다.
        void Update();

    private:
        Mutex m_taskMutex;
        // 현재 실행 중이거나 대기 중인 태스크들을 관리할 수 있습니다.
        // SMap<uint64, Ptr<Task>> m_activeTasks; 
    };
}