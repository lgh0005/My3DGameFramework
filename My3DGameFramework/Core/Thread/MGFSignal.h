#pragma once

namespace MGF3D
{
    // INFO : 윈도우의 Event 기반 순서 제어 동기화 도구
    // 와 유사한 순서 제어 동기화 도구를 구현
    MGF_CLASS_PTR(MGFSignal)
    class MGFSignal
    {
        MGF_DISABLE_COPY(MGFSignal)
        MGF_DISABLE_MOVE(MGFSignal)

    public:
        MGFSignal();
        ~MGFSignal();

    public:
        void Set();   // 신호를 보냄 (대기 중인 스레드를 깨움)
        void Wait();  // 신호가 올 때까지 현재 스레드를 정지
        bool IsSignaled() const { return m_signaled; }

    private:
        ConditionVariable m_cv;
        Mutex m_mutex;
        bool m_signaled;
    };
}