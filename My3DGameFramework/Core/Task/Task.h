#pragma once
#include "TaskDeleter.h"

namespace MGF3D
{
    using TaskUPtr = std::unique_ptr<Task, TaskDeleter>;
    using TaskPtr = std::shared_ptr<Task>;
    using TaskWPtr = std::weak_ptr<Task>;

    class Task
    {
    public:
        Task(Action<> work, Action<> onComplete = nullptr);
        ~Task();

        // 실제 작업을 수행하는 핵심 함수
        void Execute();

    private:
        Action<> m_work;       // 실제 일감
        Action<> m_onComplete; // 끝나고 할 일 (콜백)
    };
}