#include "CorePch.h"
#include "TaskDeleter.h"
#include "TaskPool.h"
#include "Task/Task.h"

namespace MGF3D
{
    void TaskDeleter::operator()(Task* task) const
    {
        // 풀 주소가 있고 태스크가 유효하다면 반납
        if (pool && task) pool->Release(task);
        else if (task)
        {
            // 만약 풀 정보가 유실되었다면 메모리 누수를 막기 위해 직접 삭제
            // (보통 발생하지 않아야 할 상황)
            delete task;
        }
    }
}