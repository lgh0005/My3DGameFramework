#include "pch.h"
#include "Managers/MemoryManager.h"
#include "Managers/NameManager.h"

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

    // [Helper] 현재 도메인별 메모리 상태 출력 람다
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
    {
        MGF_LOG_TRACE("[Step 2] Cross-Domain Move (L -> S) Analysis");

        LString lStr = "This is a temporary frame string that will be moved to Slab.";
        PrintMemState("LString Created (Frame Only)");

        SString sStr = std::move(lStr);
        PrintMemState("After Move to SString (Slab should increase)");

        MGF_ASSERT(lStr.Empty(), "Original LString must be empty!");
        MGF_ASSERT(sStr.Length() > 0, "SString must have data!");

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
            LString temp = "I will survive the frame reset!";
            survivalStr = std::move(temp);
        }

        MGF_LOG_INFO("Before Resetting Linear Pool...");
        PrintMemState("Pre-Reset");

        MemoryManager::Instance().GetLinearMemoryPool()->Reset();

        MGF_LOG_INFO("Frame Pool Reset Completed.");
        PrintMemState("Post-Reset (Linear should be 0)");

        MGF_ASSERT(MemoryManager::Instance().GetLinearMemoryPool()->GetUsedMemory() == 0, "Frame pool not reset!");
        MGF_ASSERT(!survivalStr.Empty(), "Slab data lost during frame reset!");
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

        int32* pId = assetIds.Find("Player_Model");
        MGF_ASSERT(pId && *pId == 1001, "Hash Map Search Failed!");

        PrintMemState("After StringHash Map Test");
    }

    // ---------------------------------------------------------
    // [SECTION 5] MGFName Pure String Pooling Test (Refactored)
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 5] MGFName Pure String Pooling Test");
        PrintMemState("Before MGFName Test");

        // 1. 동일한 문자열로 여러 개의 MGFName 생성
        MGFName n1("Actor");
        MGFName n2("Actor");
        MGFName n3("Actor");

        MGF_LOG_INFO("Register 'Actor' 3 times. Hashes: {0}, {1}, {2}",
            n1.GetStringHash().value, n2.GetStringHash().value, n3.GetStringHash().value);

        // 검증 1: 세 객체가 모두 완벽히 동일한 메모리 주소를 가리키는지 확인 (Pooling)
        MGF_ASSERT(n1.CStr() == n2.CStr() && n2.CStr() == n3.CStr(), "String Pooling failed! Addresses must be identical.");
        MGF_LOG_INFO("Success! All 'Actor' names share the exact same memory address.");

        // 2. 완전히 다른 이름 등록
        MGFName n4("Enemy");
        MGF_LOG_INFO("Register 'Enemy'. Hash: {0}", n4.GetStringHash().value);

        // 검증 2: 다른 이름은 다른 주소를 가져야 함
        MGF_ASSERT(n1.CStr() != n4.CStr(), "Different names should have different addresses.");

        PrintMemState("After MGFName Test");

        // 장부에는 "Actor"와 "Enemy" 단 2개만 기록되어 있어야 합니다.
        usize currentNameCount = NameManager::Instance().GetNameCount();
        MGF_LOG_INFO("Current Name Count in Pool: {0} (Should be 2)", currentNameCount);
        MGF_ASSERT(currentNameCount == 2, "Name Pool count is incorrect!");
    }

    // ---------------------------------------------------------
    // [SECTION 6] Container Iteration Test (Range-based for loop)
    // ---------------------------------------------------------
    {
        MGF_LOG_TRACE("[Step 6] Container Iteration Test");

        // 1. Vector 순회 테스트
        SVector<int32> iterVec;
        for (int i = 1; i <= 5; ++i)
        {
            iterVec.push_back(i * 10); // 10, 20, 30, 40, 50
        }

        int32 vectorSum = 0;
        // TVector에 begin(), end()를 직접 구현하지 않았어도 부모(std::vector)의 것을 자동으로 사용합니다!
        for (const int32& val : iterVec)
        {
            vectorSum += val;
        }
        MGF_ASSERT(vectorSum == 150, "Vector iteration failed!");
        MGF_LOG_INFO("SVector Iteration Success! Sum of elements: {0}", vectorSum);

        // 2. Map 순회 테스트
        SMap<StringHash, int32> iterMap;
        iterMap.Insert("Sword", 100);
        iterMap.Insert("Shield", 200);
        iterMap.Insert("Potion", 50);

        int32 mapSum = 0;
        // TMap 역시 std::unordered_map을 public 상속받았으므로 std::pair<const K, V> 형태로 순회 가능합니다.
        for (const auto& pair : iterMap)
        {
            mapSum += pair.second;
        }
        MGF_ASSERT(mapSum == 350, "Map iteration failed!");
        MGF_LOG_INFO("SMap Iteration Success! Sum of values: {0}", mapSum);

        PrintMemState("After Iteration Test");
    }

    MGF_LOG_INFO("=== All Cross-Domain Integration Tests Passed! ===");
    MGF_LOG_FLUSH();

    return 0;
}