#include "pch.h"
#include "Managers/MemoryManager.h"
#include "Managers/NameManager.h"
#include "Managers/JobManager.h"
#include "Managers/TaskManager.h"

// Debug System Headers
#include "Debug/Asserter.h"
#include "Debug/Logger.h"
#include "Debug/RealTimer.h"
#include "Debug/ProfileScope.h"
#include "Debug/MemoryProfiler.h"

// Containers
#include "Containers/Slab/SDeque.h"
#include "Containers/Slab/SVector.h"
#include "Containers/Slab/SMap.h"
#include "Containers/Slab/SSet.h"
#include "Containers/Slab/SString.h"
#include "Containers/Linear/LDeque.h"
#include "Containers/Linear/LVector.h"
#include "Containers/Linear/LMap.h"
#include "Containers/Linear/LSet.h"
#include "Containers/Linear/LString.h"

// Engine Core
#include "Identity/MGFName.h"
#include "Task/Task.h"
#include "Thread/MGFJob.h"
#include "Thread/MGFSignal.h"
#include "Utils/StringUtils.h"

using namespace MGF3D;

int main()
{
    // 1. 기초 시스템 기동
    MGF_LOG_INIT();
    MGF_TIMER_INIT();

    MGF_LOG_INFO("=== MGF3D Final Integrated System Test (Task Pooling) ===");

    {
        MGF_PROFILE_SCOPE("Full_Engine_Integration_Test");

        // 2. 엔진 매니저 초기화
        MemoryManager::Instance();

        if (!JobManager::Instance().Init())
        {
            MGF_LOG_FATAL("JobManager Initialization Failed!");
            return -1;
        }

        auto ReportMemory = [](const char* sectionName) {
            MGF_LOG_INFO(">>> Memory Report: {0}", sectionName);
            MGF_MEMORY_PROFILE_CAPTURE();
            MGF_MEMORY_LOG_STATS();
            };

        // [SECTION 1 ~ 8] 기초 테스트 생략...

        // ---------------------------------------------------------
        // [SECTION 9] Worker Thread Parallel Processing Test (Pooled)
        // ---------------------------------------------------------
        {
            MGF_LOG_TRACE("[Step 9] Parallel Task Execution Test (Pooled)");

            const int32 taskCount = 32;
            Atomic<int32> completedTasks{ 0 };

            for (int32 i = 0; i < taskCount; ++i)
            {
                // [정상화] make_unique 대신 TaskManager의 Pool을 사용합니다.
                auto task = TaskManager::Instance().AcquireTask([i, &completedTasks]() {
                    float64 val = 0.0;
                    for (int j = 0; j < 5000; ++j) val += std::sin(j);

                    MGF_LOG_INFO("[Worker] Parallel Task {0} completed.", i);
                    completedTasks.fetch_add(1);
                    });

                TaskManager::Instance().PushTask(std::move(task));
            }

            while (completedTasks.load() < taskCount) { std::this_thread::yield(); }
            MGF_LOG_INFO("Successfully processed {0} pooled parallel tasks.", taskCount);
        }

        // ---------------------------------------------------------
        // [SECTION 10] Deterministic Relay Test with Polling (Pooled)
        // ---------------------------------------------------------
        {
            MGF_LOG_TRACE("[Step 10] Deterministic Relay Test with Polling (Pooled)");

            MGFSignal renderSync;

            // [정상화] 로딩 태스크도 풀에서 빌려옵니다.
            auto loadingTask = TaskManager::Instance().AcquireTask([&renderSync]() {
                MGF_LOG_INFO("[Worker] Resource Data Loading... (CPU)");

                // [정상화] 중첩된 GPU 업로드 태스크 역시 풀에서 빌려옵니다.
                auto uploadTask = TaskManager::Instance().AcquireTask([&renderSync]() {
                    MGF_LOG_INFO("[MainThread] Executing GPU Upload Task...");
                    renderSync.Set();
                    });

                TaskManager::Instance().PushMainTask(std::move(uploadTask));
                });

            TaskManager::Instance().PushTask(std::move(loadingTask));

            MGF_LOG_INFO("Main thread processing tasks until signal...");

            while (!renderSync.IsSignaled())
            {
                TaskManager::Instance().Update();
                std::this_thread::yield();
            }

            MGF_LOG_INFO("Relay test successfully synchronized by MGFSignal!");
        }

        // 4. 메모리 리포트 (Slab 사용량 중 sizeof(Task) 버킷을 확인하세요)
        ReportMemory("Final Engine State");

        // 3. 시스템 종료
        JobManager::Instance().Shutdown();
    }

    MGF_LOG_INFO("=== All Engine Systems Passed Integration Test ===");
    MGF_LOG_FLUSH();

    return 0;
}