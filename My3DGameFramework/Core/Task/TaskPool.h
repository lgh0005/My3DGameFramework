#pragma once
#include "Task/Task.h"

namespace MGF3D
{
    class TaskPool
    {
    public:
        TaskPool(uint32 initialReserve = 1024);
        ~TaskPool();

        // 커스텀 소멸자가 장착된 안전한 UPtr 반환
        TaskUPtr Acquire(Action<> work, Action<> onComplete = nullptr);

        // 소멸자가 호출할 내부 반납 함수
        void Release(Ptr<Task> task);

    private:
        Mutex               m_lock;
        SVector<Ptr<Task>>   m_freeList;
    };
}