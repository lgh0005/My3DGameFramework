#pragma once
#include "Thread/MGFThreadContext.h"
#include "Containers/Slab/SMap.h"
#include "Containers/Slab/SString.h"

namespace MGF3D
{
    class ThreadManager
    {
        MGF_DECLARE_SINGLE(ThreadManager)

    private:
        ThreadManager();
        ~ThreadManager();

    public:
        // 스레드를 생성하고 관리 목록에 등록합니다.
        bool CreateThread(const SString& name, Action<> work);

        // 특정 스레드의 준비 신호를 대기합니다.
        void WaitUntilReady(const SString& name);

        // 특정 스레드가 준비되었음을 알립니다. (스레드 내부에서 호출)
        void NotifyReady(const SString& name);

    private:
        Mutex m_registryMutex;
        SMap<StringHash, MGFThreadContextPtr> m_threadMap;
    };
}