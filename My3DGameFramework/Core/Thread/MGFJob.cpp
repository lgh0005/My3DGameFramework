#include "CorePch.h"
#include "Thread/MGFJob.h"
#include "Thread/MGFThread.h"
#include "Thread/MGFSignal.h"

namespace MGF3D
{
    MGFJob::MGFJob()
    {
        m_thread = MakeUnique<MGFThread>();
        m_readySignal = MakeUnique<MGFSignal>();
    }

    MGFJob::~MGFJob()
    {
        Join();
    }

    bool MGFJob::Launch(const SString& name, Action<> work)
    {
        if (!m_thread) return false;
        return m_thread->Start(name, work);
    }

    void MGFJob::NotifyReady()
    {
        if (m_readySignal) m_readySignal->Set();
    }

    void MGFJob::WaitUntilReady()
    {
        if (m_readySignal) m_readySignal->Wait();
    }

    void MGFJob::Join()
    {
        if (m_thread) m_thread->Join();
    }
}