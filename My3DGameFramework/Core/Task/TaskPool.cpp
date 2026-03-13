#include "CorePch.h"
#include "TaskPool.h"

namespace MGF3D
{
    TaskPool::TaskPool(uint32 initialReserve)
    {
        m_freeList.Reserve(initialReserve);
    }

    TaskPool::~TaskPool()
    {
        // 1. 소멸 시 리스트 정리(전체 범위 락)
        {
            MGF_LOCK_SCOPE(m_lock);
            m_freeList.Clear();
        }
    }

    TaskUPtr TaskPool::Acquire(Action<> work, Action<> onComplete)
    {
        Task* task = nullptr;

        // 2. 프리 리스트에서 꺼내올 때만 짧게 락을 겁니다.
        {
            MGF_LOCK_SCOPE(m_lock);
            if (!m_freeList.Empty())
                task = m_freeList.PopBack();
        }

        if (task)
        {
            // 기존 메모리 영역에 생성자만 다시 호출 (재초기화)
            new (task) Task(std::move(work), std::move(onComplete));
        }
        else
        {
            // 풀이 비었으면 Slab에서 새로 할당
            void* mem = MemoryManager::Instance().Allocate(sizeof(Task));
            task = new (mem) Task(std::move(work), std::move(onComplete));
        }

        // 빌려줄 때 '나(this)'를 알고 있는 델리터와 함께 보냅니다.
        return TaskUPtr(task, TaskDeleter{ this });
    }

    void TaskPool::Release(Task* task)
    {
        if (!task) return;

        // 객체의 소멸자만 명시적으로 호출
        // Action<> 내부의 std::function 등이 잡고 있는 자원을 정리하기 위함입니다.
        task->~Task();

        // 4. 다시 리스트에 넣을 때만 락을 겁니다.
        {
            MGF_LOCK_SCOPE(m_lock);
            m_freeList.PushBack(task);
        }
    }
}