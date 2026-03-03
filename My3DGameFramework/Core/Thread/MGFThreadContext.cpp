#include "CorePch.h"
#include "Thread/MGFThreadContext.h"
#include "Thread/MGFThread.h"
#include "Thread/MGFSignal.h"

namespace MGF3D
{
    MGFThreadContext::MGFThreadContext()
    {
        m_thread = std::make_unique<MGFThread>();
        m_readySignal = std::make_unique<MGFSignal>();
    }

    MGFThreadContext::~MGFThreadContext() = default;

    bool MGFThreadContext::Launch(const SString& name, Action<> work)
    {
        if (!m_thread) return false;
        return m_thread->Start(name, work);
    }

    void MGFThreadContext::NotifyReady()
    {
        if (m_readySignal) m_readySignal->Set();
    }

    void MGFThreadContext::WaitUntilReady()
    {
        if (m_readySignal) m_readySignal->Wait();
    }
}