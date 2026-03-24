#include "CorePch.h"
#include <new>
#include "Managers/MemoryManager.h"
#include "TaskPool.h"

namespace MGF3D
{
    TaskPool::TaskPool(uint32 initialReserve)
    {
        MGF_LOCK_SCOPE(m_lock);
        m_freeList.Reserve(initialReserve);
    }

    TaskPool::~TaskPool()
    {
        // 1. 파괴 시점에 다른 스레드가 접근하지 못하도록 락
        MGF_LOCK_SCOPE(m_lock);

        // 2. 프리 리스트에 남아있는 메모리 블록들을 해제
        for (Task* task : m_freeList)
        {
            if (task)
                MGF_MEMORY.Deallocate(task, sizeof(Task));
        }

        m_freeList.Clear();
        MGF_LOG_INFO("TaskPool: Destroyed and cleared all pooled memory.");
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
            // [Case A] 기존 메모리 재사용
            // 이미 확보된 메모리 영역에 생성자만 다시 실행하여 객체를 깨웁니다.
            ::new (task) Task(std::move(work), std::move(onComplete));
        }
        else
        {
            // [Case B] 풀이 비어있음 -> 새로 할당
            // Task가 PoolAlloc을 상속받았으므로, new Task는 내부적으로 
            // MGF_MEMORY.Allocate를 호출하고 생성자까지 완료합니다.
            task = new Task(std::move(work), std::move(onComplete));
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