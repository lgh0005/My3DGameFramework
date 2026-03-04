#pragma once
#include "Thread/MGFJob.h"

namespace MGF3D
{
    class JobManager
    {
        MGF_DECLARE_SINGLE(JobManager)

    private:
        JobManager();
        ~JobManager();

    public:
        bool Init();
        void Shutdown();
        bool IsShutdown() const { return m_isShutdown.load(); }

    private:
        void WorkerLoop(MGFJob* self);
        bool CreateWorker();

        SVector<MGFJobUPtr> m_workers;
        Atomic<bool> m_isShutdown  { false };
    };
}