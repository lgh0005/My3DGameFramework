#include "pch.h"
#include "Managers/MemoryManager.h"

// Debug System Headers
#include "Debuggers/Asserter.h"
#include "Debuggers/Logger.h"
#include "Debuggers/RealTimer.h"
#include "Debuggers/ProfileScope.h"

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
    // 1. 디버그 시스템 기동 (로거 및 타이머 초기화)
    MGF_LOG_INIT();
    MGF_TIMER_INIT();

    MGF_LOG_INFO("=== MGF3D Container Series (S vs L) Full Test ===");

    // 전체 테스트 시간 측정 시작
    MGF_PROFILE_SCOPE("Total_Container_Test");

    // 2. 엔진 메모리 시스템 가동
    {
        MGF_PROFILE_SCOPE("MemoryManager_Initialization");
        MemoryManager::Instance();
    }

    // ---------------------------------------------------------
    // [SECTION 1] Linear (L) 시리즈 테스트
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 1] Linear Series (Frame Memory) Test - Start");
        MGF_PROFILE_START(LinearSection);

        usize beforeFrame = MemoryManager::Instance().GetFramePool()->GetUsedMemory();

        LVector<RenderData> lVec;
        LMap<int32, RenderData> lMap;
        LSet<uint64> lSet;

        lVec.push_back(RenderData{ {}, 1 });
        lMap.Insert(10, RenderData{ {}, 10 });
        lSet.Insert(100);

        usize afterFrame = MemoryManager::Instance().GetFramePool()->GetUsedMemory();

        MGF_LOG_INFO("L-Vector Element Addr : {0}", (void*)&lVec[0]);
        MGF_LOG_INFO("Allocated Frame Memory: {0} bytes", (afterFrame - beforeFrame));

        MGF_PROFILE_END(LinearSection);
    }

    // ---------------------------------------------------------
    // [SECTION 2] Slab (S) 시리즈 테스트
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 2] Slab Series (Persistent Memory) Test - Start");
        MGF_PROFILE_SCOPE("Slab_Test_Scope");

        SVector<RenderData> sVec;
        SMap<int32, RenderData> sMap;
        SSet<uint64> sSet;

        sVec.push_back(RenderData{ {}, 2 });
        sMap.Insert(20, RenderData{ {}, 20 });
        sSet.Insert(200);

        // 정렬 상태 어설션 테스트
        uintptr_t addr = reinterpret_cast<uintptr_t>(&sVec[0]);
        MGF_ASSERT(addr % 32 == 0, "S-Vector memory must be 32-byte aligned!");

        MGF_LOG_INFO("S-Vector Addr : {0} (32-byte Align Verified)", (void*)addr);
        MGF_LOG_INFO("S-Vector Memory Usage : {0} bytes", sVec.MemoryUsage());
    }

    // ---------------------------------------------------------
    // [SECTION 3] 중첩 구조 및 프레임 리셋 테스트
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 3] Nested & Reset Test - Start");

        LMap<int32, SVector<RenderData>> frameCache;

        // 수동 타임스탬프 측정 예시
        uint64 ts_start = MGF_TIMER_GET_TS();

        SVector<RenderData> persistentData;
        persistentData.push_back(RenderData{ {}, 999 });
        frameCache.Insert(0, std::move(persistentData));

        // 프레임 종료 시점 시뮬레이션
        MemoryManager::Instance().GetFramePool()->Reset();

        uint64 ts_end = MGF_TIMER_GET_TS();

        MGF_LOG_INFO("Frame Reset Done. Used Frame Memory: {0} bytes",
            MemoryManager::Instance().GetFramePool()->GetUsedMemory());

        MGF_LOG_INFO("Reset Logic took {0} seconds", MGF_TIMER_ELAPSED(ts_start, ts_end));
    }

    MGF_LOG_INFO("All Container Tests Passed Successfully.");

    // ---------------------------------------------------------
    // [SECTION 4] String Series (S vs L) 테스트
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 4] String Series (Slab & Linear) Test - Start");
        MGF_PROFILE_SCOPE("String_Test_Scope");

        // 1. SString (Slab) - 영구 보관용 문자열 테스트
        {
            // SSO(Small String Optimization) 테스트: 15자 이하
            MGF3D::SString shortStr = "Short_SSO";
            // Heap 할당 테스트: 32자 이상 (SSO 임계값 초과 유도)
            MGF3D::SString longStr = "This is a very long string that must be allocated in the Slab Pool because it exceeds SSO limits.";

            MGF_LOG_INFO("S-String (Short) Memory: {0} bytes (Expected: Object Size only)", shortStr.MemoryUsage());
            MGF_LOG_INFO("S-String (Long)  Memory: {0} bytes (Expected: Object Size + Heap)", longStr.MemoryUsage());

            MGF_ASSERT(shortStr.Length() == 9, "SString length mismatch!");
            MGF_ASSERT(longStr.Contains("Slab Pool"), "SString 'Contains' logic failed!");
        }

        // 2. LString (Linear) - 프레임 임시 문자열 테스트
        {
            usize beforeFrame = MemoryManager::Instance().GetFramePool()->GetUsedMemory();

            MGF3D::LString frameStr = "Frame_Temporary_String_Data";
            MGF3D::LString frameStr2 = "Another_Frame_String";

            usize afterFrame = MemoryManager::Instance().GetFramePool()->GetUsedMemory();

            MGF_LOG_INFO("L-String Content: {0}", frameStr.CStr());
            MGF_LOG_INFO("L-Strings Allocated in Frame Pool: {0} bytes", (afterFrame - beforeFrame));

            // Capacity 확인
            MGF_LOG_INFO("L-String Capacity: {0}", frameStr.Capacity());
        }

        MGF_LOG_INFO("String Tests Completed.");
    }

    // ---------------------------------------------------------
    // [SECTION 5] StringHash & Map Integration Test
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 5] StringHash & Map Integration - Start");
        MGF_PROFILE_SCOPE("StringHash_Map_Test");

        // 1. StringHash를 키로 사용하는 Slab 기반 Map 생성
        // 이제 이 맵은 문자열 비교를 하지 않고, FNV-1a 정수값으로만 데이터를 찾습니다.
        SMap<StringHash, RenderData> assetMap;

        // 2. 데이터 삽입 (문자열 리터럴 -> StringHash 자동 변환/컴파일 타임 해싱)
        assetMap.Insert("Warrior_Model", RenderData{ {}, 101 });
        assetMap.Insert("Dragon_Texture", RenderData{ {}, 202 });
        assetMap.Insert("Level_01_Base", RenderData{ {}, 303 });

        // 3. 다양한 방식으로 데이터 찾기

        // A. 리터럴로 즉시 찾기 (컴파일 타임 해싱 활용)
        RenderData* pData1 = assetMap.Find("Warrior_Model");

        // B. SString 객체에서 해시를 뽑아서 찾기 (런타임 해싱 활용)
        SString searchName = "Dragon_Texture";
        RenderData* pData2 = assetMap.Find(searchName.Hash());

        // C. 미리 구워진 constexpr 해시로 찾기
        constexpr StringHash staticKey = "Level_01_Base";
        RenderData* pData3 = assetMap.Find(staticKey);

        // 4. 검증 및 로그 출력
        MGF_ASSERT(pData1 && pData1->id == 101, "Find by Literal failed!");
        MGF_ASSERT(pData2 && pData2->id == 202, "Find by GetHash() failed!");
        MGF_ASSERT(pData3 && pData3->id == 303, "Find by Constexpr Key failed!");

        MGF_LOG_INFO("StringHash 'Warrior_Model' Value: 0x{0:x}", (uint32)StringHash("Warrior_Model"));
        MGF_LOG_INFO("StringHash Map Search Successfully Verified. (Integer Comparison Only)");
    }

    // 로그 버퍼 비우기
    MGF_LOG_FLUSH();

    return 0;
}