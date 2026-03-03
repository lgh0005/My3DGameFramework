#include "CorePch.h"
#include "MGFThread.h"

namespace MGF3D
{
	MGFThread::MGFThread() = default;
	MGFThread::~MGFThread()
	{
		if (IsJoinable()) m_thread.join();
	}

    bool MGFThread::Start(const SString& name, Action<> work)
    {
        if (m_thread.joinable()) return false;

        m_name = name;
        m_thread = MGF3D::Thread([this, work]()
        {
            if (work) work();
        });

        return true;
    }

    bool MGFThread::Start(const SString& name, PAction<> work)
    {
        if (m_thread.joinable()) return false;

        m_name = name;
        m_thread = MGF3D::Thread([this, work]()
        {
            if (work) work();
        });

        return true;
    }

    void MGFThread::Join()
    {
        if (IsJoinable()) m_thread.join();
    }

    void MGFThread::Detach()
    {
        if (IsJoinable()) m_thread.detach();
    }
}