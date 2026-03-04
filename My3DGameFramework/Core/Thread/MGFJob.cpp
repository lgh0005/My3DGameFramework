#include "CorePch.h"
#include "Thread/MGFJob.h"
#include "Thread/MGFThread.h"
#include "Thread/MGFSignal.h"

namespace MGF3D
{
    MGFJob::MGFJob()
    {
        m_thread = std::make_unique<MGFThread>();
        m_readySignal = std::make_unique<MGFSignal>();
    }

    MGFJob::~MGFJob() = default;

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
}