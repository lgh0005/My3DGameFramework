#include "CorePch.h"
#include "ThreadManager.h"

namespace MGF3D
{
    ThreadManager::ThreadManager() = default;
    ThreadManager::~ThreadManager() = default;

    bool ThreadManager::CreateThread(const SString& name, Action<> work)
    {
        // 1. 컨텍스트 생성 (소유권은 이 함수가 잠시 가짐)
        auto context = std::make_unique<MGFThreadContext>();

        // 2. 스레드 구동 시도
        if (!context->Launch(name, std::move(work)))
            return false;

        // 3. 관리 맵에 등록 (소유권을 맵으로 이전)
        MGF_LOCK_SCOPE
        {
            Lock lock(m_registryMutex);
            // unique_ptr를 Move하여 맵이 유일한 소유자가 되게 합니다.
            m_threadMap.Insert(StringHash(name), std::move(context));
        }

        return true;
    }

    void ThreadManager::WaitUntilReady(const SString& name)
    {
        Ptr<MGFThreadContext> target = nullptr;

        MGF_LOCK_SCOPE
        {
            Lock lock(m_registryMutex);
            auto it = m_threadMap.Find(StringHash(name));
            if (it) target = (*it).get();
        }
        
        if (target)
            target->WaitUntilReady();
    }

    void ThreadManager::NotifyReady(const SString& name)
    {
        Ptr<MGFThreadContext> target = nullptr;

        MGF_LOCK_SCOPE
        {
            Lock lock(m_registryMutex);
            auto it = m_threadMap.Find(StringHash(name));
            if (it) target = (*it).get();
        }

        if (target)
            target->NotifyReady();
    }
}