#include "CorePch.h"
#include "TaskPool.h"

namespace MGF3D
{
    TaskPool::TaskPool(uint32 initialReserve)
    {
        m_freeList.reserve(initialReserve);
    }

    TaskPool::~TaskPool()
    {
        MGF_LOCK_SCOPE
        {
            Lock lock(m_lock);
            m_freeList.Clear();
        }
    }

    TaskUPtr TaskPool::Acquire(Action<> work, Action<> onComplete)
    {
        Task* task = nullptr;

        MGF_LOCK_SCOPE
        {
            Lock lock(m_lock);
            if (!m_freeList.Empty())
                task = m_freeList.PopBack();
        }

        if (task)
        {
            // [정상화] 기존 메모리 영역에 생성자만 다시 호출 (재초기화)
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

        MGF_LOCK_SCOPE
        {
            Lock lock(m_lock);
            m_freeList.PushBack(task);
        }
    }
}