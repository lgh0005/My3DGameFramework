#include "CorePch.h" 

#include "Managers/MemoryManager.h"
#include "Managers/NameManager.h"
#include "Managers/JobManager.h"
#include "Managers/TaskManager.h"
#include "Managers/PathManager.h"

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

        // 2. 엔진 매니저 초기화 (순서 중요!)
        MemoryManager::Instance(); // 메모리 풀 준비

        // 워커 스레드 생성 (가장 먼저 일꾼을 준비해야 함)
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

        // ---------------------------------------------------------
        // [SECTION 9] Worker Thread Parallel Processing Test (Pooled)
        // ---------------------------------------------------------
        {
            MGF_LOG_TRACE("[Step 9] Parallel Task Execution Test (Pooled)");

            const int32 taskCount = 32;
            Atomic<int32> completedTasks{ 0 };

            for (int32 i = 0; i < taskCount; ++i)
            {
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

            auto loadingTask = TaskManager::Instance().AcquireTask([&renderSync]() {
                MGF_LOG_INFO("[Worker] Resource Data Loading... (CPU)");

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

        // ---------------------------------------------------------
        // [SECTION 11] PathManager & JsonParser Integration Test
        // ---------------------------------------------------------
        // [중요] JobManager가 살아있을 때 PathManager 테스트를 수행해야 합니다!
        {
            MGF_LOG_TRACE("[Step 11] PathManager Virtual Path Resolution Test");

            // 1. PathManager 초기화 (비동기 LoadConfig 수행 -> 워커 스레드 필요함)
            if (!PathManager::Instance().Init())
            {
                MGF_LOG_FATAL("PathManager: Initialization failed!");
                // 여기서 실패해도 일단 Shutdown 순서는 지키는 게 좋음
            }
            else
            {
                PathManager::Instance().DebugDumpMap();

                // 2. 가상 경로 해석 테스트 (@Assets가 Config에 있다고 가정)
                // 만약 Config 파일에 @GameAsset이 없다면 @Assets나 @Config로 테스트하세요.
                MGFPath testPath = PathManager::Instance().Resolve("@Assets/Textures/Hero.png");
                SString resolvedStr = testPath.ToSString();

                MGF_LOG_INFO(">>> Path Resolution Result:");
                MGF_LOG_INFO("  [Input]  @Assets/Textures/Hero.png");
                MGF_LOG_INFO("  [Output] {0}", resolvedStr.CStr());

                // 3. 검증 (실제 경로가 포함되어 있는지)
                // EngineConfig.json의 실제 경로 설정에 따라 기대값을 맞춰주세요.
                // 예: "C:/MyGame/Assets/Textures/Hero.png"
                if (resolvedStr.Contains("Assets/Textures/Hero.png"))
                {
                    MGF_LOG_INFO("PathManager: [Verification Success] Path Resolved Correctly.");
                }
                else
                {
                    MGF_LOG_WARN("PathManager: [Verification Warning] Output path might differ from expectation.");
                }
            }
        }

        // ---------------------------------------------------------
        // [SYSTEM SHUTDOWN]
        // ---------------------------------------------------------
        // 4. 메모리 리포트 (최종 상태)
        ReportMemory("Final Engine State");

        MGF_LOG_TRACE("Initiating System Shutdown...");

        // [중요] 켜는 순서의 역순(FILO)으로 끕니다.

        // 1. PathManager 종료 (더 이상 경로 조회 안 함)
        PathManager::Instance().Shutdown();

        // 2. JobManager 종료 (워커 스레드 해산)
        // 이걸 먼저 끄면 위에서 비동기 작업이 남았을 때 데드락 걸립니다.
        JobManager::Instance().Shutdown();

        MGF_LOG_INFO("All Systems Shutdown Successfully.");
    }

    MGF_LOG_INFO("=== All Engine Systems Passed Integration Test ===");
    MGF_LOG_FLUSH();

    return 0;
}