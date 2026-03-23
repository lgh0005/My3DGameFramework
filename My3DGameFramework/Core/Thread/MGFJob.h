#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
    MGF_CLASS_PTR(MGFThread)
    MGF_CLASS_PTR(MGFSignal)

    MGF_CLASS_PTR(MGFJob)
    class MGFJob : public PoolAlloc
    {
        MGF_DISABLE_COPY(MGFJob)
        MGF_DISABLE_MOVE(MGFJob)

    public:
        MGFJob();
        virtual ~MGFJob() override;

    public:
        // 스레드를 실제로 구동합니다.
        bool Launch(const SString& name, Action<> work);

        // 스레드 내부에서 "나 준비 완료!"라고 외칠 때 호출합니다.
        void NotifyReady();

        // 외부(주로 메인 스레드)에서 이 스레드가 준비될 때까지 기다립니다.
        void WaitUntilReady();

        Ptr<MGFThread> GetThread() const { return m_thread.Get(); }

    private:
        MGFThreadUPtr m_thread;      // 실제 스레드 객체
        MGFSignalUPtr m_readySignal; // 부팅 완료를 알리는 시그널
    };
}