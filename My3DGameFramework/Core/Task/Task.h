#pragma once

#pragma once
#include "Common/Delegates.h"

namespace MGF3D
{
    class Task
    {
    public:
        Task(Action<> work, Action<> onComplete = nullptr);
        virtual ~Task();

        // 실제 작업을 수행하는 핵심 함수
        void Execute();

    private:
        Action<> m_work;       // 실제 일감
        Action<> m_onComplete; // 끝나고 할 일 (콜백)
    };
}