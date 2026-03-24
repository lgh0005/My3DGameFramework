#pragma once
#include <memory>

namespace MGF3D
{
    class Task;
    class TaskPool;

    // 태스크를 파괴하지 않고 풀로 반납하는 전용 델리터
    struct TaskDeleter
    {
        Ptr<TaskPool> pool { nullptr };
        void operator()(Task* task) const;
    };
}