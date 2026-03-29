#pragma once
#include "Task/Task.h"
#include "Task/TaskPool.h"

// TODO : 지금은 Lock-base management를 택하고 있지만
// 이후에는 Lock-free 자료구조를 택하는 식으로 바뀌어야 할 것이다.

namespace MGF3D
{
    class TaskManager
    {
        MGF_DECLARE_SINGLE(TaskManager)

    private:
        TaskManager();
        ~TaskManager();

    public:
        void Shutdown();

    /*================//
    //  task pooling  //
    //================*/
    public:
        TaskUPtr AcquireTask(Action<> work, Action<> onComplete = nullptr);

    /*========================//
    //  for main thread task  //
    //========================*/
    public:
        // 2. 메인 스레드 전용 (Commit 단계용)
        // 메인 스레드(OpenGL 컨텍스트 등)에서 실행되어야 할 태스크를 등록합니다.
        void PushMainTask(TaskUPtr task);

    /*========================//
    //  for main thread task  //
    //========================*/
    public:
        // 1. 워커 스레드용 (Job 시스템)
        // 새로운 태스크를 워커 큐에 등록하고 잠든 워커를 깨웁니다.
        void PushTask(TaskUPtr task);

        // 워커가 일감을 하나 꺼내갑니다.
        TaskUPtr PopTask();

        // 일감이 없을 때 워커를 대기 상태로 만듭니다.
        void WaitForTask();

        // 모든 워커를 강제로 깨웁니다. (종료 시 사용)
        void Broadcast();

        // 메인 루프에서 호출되어 메인 전용 태스크들을 처리합니다.
        void Update();

    private:
        // 태스크 풀
        TaskPool          m_taskPool;

        // 워커 큐 관련
        Mutex             m_queueMutex;
        ConditionVariable m_condition;
        SDeque<TaskUPtr> m_taskQueue;

        // 메인 큐 관련
        // TODO : 이 부분은 없어도 될 것 같기도 함.
        // 어차피 AssetManager랑 ResourceManger는 이걸 쓰긴 할텐데
        // 그건 이미 각 매니저 내부에 구현을 해둘 예정임.
        // 그런데 다른 데서 쓰고 있을 수 있음.
        Mutex             m_mainMutex;
        SDeque<TaskUPtr> m_mainQueue;
    };
}