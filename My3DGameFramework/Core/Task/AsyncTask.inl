#pragma once
#include "Managers/ThreadManager.h"

namespace MGF3D
{
    template<typename T>
    inline MGFFuture<T> AsyncTask::Run(Func<T> work)
    {
        // 1. 결과물을 담을 약속(Promise) 생성
        // 스레드 사이에서 안전하게 공유하기 위해 shared_ptr로 관리합니다.
        auto promise = std::make_shared<MGFPromise<T>>();
        auto future = promise->GetFuture();

        // 2. 일감을 Task 박스로 포장하여 실행
        // 람다 내부에서 작업을 수행하고 결과를 Promise에 채웁니다.
        Run([promise, work]() mutable {
            if (work)
            {
                promise->SetValue(work());
            }
            });

        return future;
    }

    inline void AsyncTask::Run(Action<> work, Action<> onComplete)
    {
        // 3. 단순 Action 작업도 Task 객체로 캡슐화하여 전달합니다.
        // 나중에 TaskManager가 이 객체들을 추적할 수 있게 됩니다.
        auto task = std::make_shared<Task>(std::move(work), std::move(onComplete));

        // 우선은 ThreadManager를 통해 즉시 비동기 실행을 요청합니다.
        ThreadManager::Instance().CreateThread("AsyncTask", [task]() {
            task->Execute();
            });
    }
}