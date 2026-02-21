#include "pch.h"
#include "Managers/MemoryManager.h"
#include "Managers/NameManager.h"

// Debug System Headers
#include "Debug/Asserter.h"
#include "Debug/Logger.h"
#include "Debug/RealTimer.h"
#include "Debug/ProfileScope.h"
#include "Debug/MemoryProfiler.h"

// Slab 시리즈
#include "Containers/Slab/SVector.h"
#include "Containers/Slab/SMap.h"
#include "Containers/Slab/SSet.h"
#include "Containers/Slab/SString.h"

// Linear 시리즈
#include "Containers/Linear/LVector.h"
#include "Containers/Linear/LMap.h"
#include "Containers/Linear/LSet.h"
#include "Containers/Linear/LString.h"

// 이름 시리즈
#include "Naming/MGFName.h"

// Utils
#include "Utils/StringUtils.h"

using namespace MGF3D;

// 테스트용 구조체 (64바이트, 32바이트 정렬)
struct alignas(32) RenderData
{
    float32 matrix[16];
    uint64 id;

    bool operator==(const RenderData& other) const { return id == other.id; }
};

// 해시 특수화
namespace std {
    template<> struct hash<RenderData> {
        size_t operator()(const RenderData& d) const { return hash<uint64>{}(d.id); }
    };
}

int main()
{
    // 1. 디버그 시스템 기동
    MGF_LOG_INIT();
    MGF_TIMER_INIT();

    MGF_LOG_INFO("=== MGF3D Domain-Integrated Container Test ===");

    // 전체 테스트 시간 측정 시작
    MGF_PROFILE_SCOPE("Total_Container_Test");

    // 2. 엔진 메모리 시스템 가동
    MemoryManager::Instance();

    // [Helper] 프로파일러 캡처 및 로그 출력 매크로 활용
    auto ReportMemory = [](const char* sectionName) {
        MGF_LOG_INFO(">>> Memory Report: {0}", sectionName);
        MGF_MEMORY_PROFILE_CAPTURE();
        MGF_MEMORY_LOG_STATS();
        };

    // ---------------------------------------------------------
    // [SECTION 1] 기초 컨테이너 동작 확인
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 1] Basic Container Operation Test");
        ReportMemory("Initial State");

        LVector<RenderData> lVec;
        lVec.push_back(RenderData{ {}, 1 });

        SVector<RenderData> sVec;
        sVec.push_back(RenderData{ {}, 2 });

        ReportMemory("After Basic Push (L/S Vector)");
    }

    // ---------------------------------------------------------
    // [SECTION 2] Cross-Domain Move (L -> S) 상세 분석
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 2] Cross-Domain Move (L -> S) Analysis");

        LString lStr = "This is a temporary frame string that will be moved to Slab.";
        SString sStr = std::move(lStr);

        LVector<RenderData> lVec;
        for (int i = 0; i < 50; ++i) lVec.push_back(RenderData{ {}, (uint64)i });

        SVector<RenderData> sVec = std::move(lVec);

        ReportMemory("After L-to-S Domain Move");
    }

    // ---------------------------------------------------------
    // [SECTION 3] 프레임 리셋 후 생존성 테스트
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 3] Persistence Verification after Frame Reset");

        SString survivalStr;
        {
            LString temp = "I will survive the frame reset!";
            survivalStr = std::move(temp);
        }

        MemoryManager::Instance().GetLinearMemoryPool()->Reset();

        ReportMemory("After Linear Pool Reset");
        MGF_LOG_INFO("Verified Survivor Data: {0}", survivalStr.CStr());
    }

    // ---------------------------------------------------------
    // [SECTION 4] StringHash Map Integration
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 4] StringHash Map Performance Test");

        SMap<StringHash, int32> assetIds;
        assetIds.Insert("Player_Model", 1001);
        assetIds.Insert("Enemy_Model", 2002);

        ReportMemory("After StringHash Map Test");
    }

    // ---------------------------------------------------------
    // [SECTION 5] MGFName Pure String Pooling Test (Refactored)
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 5] MGFName Pure String Pooling Test");

        MGFName n1("Actor");
        MGFName n2("Actor");
        MGFName n3("Actor");
        MGFName n4("Enemy");

        ReportMemory("After MGFName Pooling Test");

        usize currentNameCount = NameManager::Instance().GetNameCount();
        MGF_LOG_INFO("Current Name Count in Pool: {0}", currentNameCount);
    }

    // ---------------------------------------------------------
    // [SECTION 6] Container Iteration Test
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 6] Container Iteration Test");

        SVector<int32> iterVec;
        for (int i = 1; i <= 5; ++i) iterVec.push_back(i * 10);

        SMap<StringHash, int32> iterMap;
        iterMap.Insert("Sword", 100);
        iterMap.Insert("Shield", 200);

        ReportMemory("Final State (After Iteration Test)");
    }

    MGF_LOG_INFO("=== All Cross-Domain Integration Tests Passed! ===");
    MGF_LOG_FLUSH();

    return 0;
}