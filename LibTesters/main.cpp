#include "pch.h"
#include "Managers/MemoryManager.h"

// Debug System Headers
#include "Debug/Asserter.h"
#include "Debug/Logger.h"
#include "Debug/RealTimer.h"
#include "Debug/ProfileScope.h"

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

    // [Helper] 현재 도메인별 메모리 상태 출력 람다
    // 파트너님이 수정하신 GetTotalSlabUsedMemory()와 GetLinearMemoryPool()을 사용합니다.
    auto PrintMemState = [](const char* label) {
        auto& memMgr = MemoryManager::Instance();
        usize sUsed = memMgr.GetTotalSlabUsedMemory();
        usize lUsed = memMgr.GetLinearMemoryPool()->GetUsedMemory();

        MGF_LOG_INFO(">> [{0}] | Slab: {1} bytes | Linear: {2} bytes", label, sUsed, lUsed);
        };

    // ---------------------------------------------------------
    // [SECTION 1] 기초 컨테이너 동작 확인
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 1] Basic Container Operation Test");
        PrintMemState("Initial State");

        LVector<RenderData> lVec;
        lVec.push_back(RenderData{ {}, 1 });
        PrintMemState("After LVector Push");

        SVector<RenderData> sVec;
        sVec.push_back(RenderData{ {}, 2 });
        PrintMemState("After SVector Push");
    }

    // ---------------------------------------------------------
    // [SECTION 2] Cross-Domain Move (L -> S) 상세 분석
    // ---------------------------------------------------------
    // 프레임 메모리에서 가공된 데이터가 영구 풀로 이동할 때의 메모리 변화를 관찰합니다.
    {
        MGF_LOG_TRACE("[Step 2] Cross-Domain Move (L -> S) Analysis");

        // 1. LString 생성 (Frame Pool 점유)
        LString lStr = "This is a temporary frame string that will be moved to Slab.";
        PrintMemState("LString Created (Frame Only)");

        // 2. SString으로 이동 (Cross-Allocator Move)
        // 이때 Slab 사용량은 늘어나고, Frame 사용량은 (Reset 전이므로) 그대로여야 합니다.
        SString sStr = std::move(lStr);
        PrintMemState("After Move to SString (Slab should increase)");

        MGF_ASSERT(lStr.Empty(), "Original LString must be empty!");
        MGF_ASSERT(sStr.Length() > 0, "SString must have data!");

        // 3. LVector -> SVector 대량 이동 테스트
        LVector<RenderData> lVec;
        for (int i = 0; i < 50; ++i) lVec.push_back(RenderData{ {}, (uint64)i });
        PrintMemState("L-Vector 50 elements created");

        SVector<RenderData> sVec = std::move(lVec);
        PrintMemState("After L-to-S Vector Move");

        MGF_ASSERT(lVec.empty(), "Original LVector cleared");
        MGF_ASSERT(sVec.size() == 50, "SVector successfully received elements");
    }

    // ---------------------------------------------------------
    // [SECTION 3] 프레임 리셋 후 생존성 테스트
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 3] Persistence Verification after Frame Reset");

        SString survivalStr;
        {
            // 임시 스코프에서 프레임 문자열 가공
            LString temp = "I will survive the frame reset!";
            survivalStr = std::move(temp); // S도메인으로 승격(Move)
        }

        MGF_LOG_INFO("Before Resetting Linear Pool...");
        PrintMemState("Pre-Reset");

        // 프레임 메모리 전체 날림
        MemoryManager::Instance().GetLinearMemoryPool()->Reset();

        MGF_LOG_INFO("Frame Pool Reset Completed.");
        PrintMemState("Post-Reset (Linear should be 0)");

        // 결과 확인: Linear는 0이지만 survivalStr의 데이터는 Slab에 있으므로 안전해야 함
        MGF_ASSERT(MemoryManager::Instance().GetLinearMemoryPool()->GetUsedMemory() == 0, "Frame pool not reset!");
        MGF_ASSERT(!survivalStr.Empty(), "Slab data lost during frame reset!");
        MGF_LOG_INFO("Verified Survivor Data: {0}", survivalStr.CStr());
    }

    // ---------------------------------------------------------
    // [SECTION 4] StringHash & Map Integration
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 4] StringHash Map Performance Test");

        SMap<StringHash, int32> assetIds;
        assetIds.Insert("Player_Model", 1001);
        assetIds.Insert("Enemy_Model", 2002);

        // 검색 시 문자열 비교가 아닌 정수 해시 비교만 발생
        int32* pId = assetIds.Find("Player_Model");
        MGF_ASSERT(pId && *pId == 1001, "Hash Map Search Failed!");

        PrintMemState("After StringHash Map Test");
    }

    MGF_LOG_INFO("=== All Cross-Domain Integration Tests Passed! ===");
    MGF_LOG_FLUSH();

    return 0;
}