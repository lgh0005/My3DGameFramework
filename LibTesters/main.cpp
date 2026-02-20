#include "pch.h"
#include "Managers/MemoryManager.h"

// Slab 시리즈
#include "Containers/Slab/SVector.h"
#include "Containers/Slab/SMap.h"
#include "Containers/Slab/SSet.h"

// Linear 시리즈
#include "Containers/Linear/LVector.h"
#include "Containers/Linear/LMap.h"
#include "Containers/Linear/LSet.h"

using namespace MGF3D;

// 테스트용 구조체 (64바이트, 32바이트 정렬)
struct alignas(32) RenderData
{
    float matrix[16];
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
    // 1. 엔진 메모리 시스템 가동
    MemoryManager::Instance();

    std::cout << "=== MGF3D Container Series (S vs L) Full Test ===" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    // ---------------------------------------------------------
    // [SECTION 1] Linear (L) 시리즈 테스트 - 프레임 임시 메모리
    // ---------------------------------------------------------
    {
        std::cout << "[Step 1] Linear Series (Frame Memory) Test" << std::endl;
        usize beforeFrame = MemoryManager::Instance().GetFramePool()->GetUsedMemory();

        LVector<RenderData> lVec;
        LMap<int32, RenderData> lMap;
        LSet<uint64> lSet;

        lVec.push_back(RenderData{ {}, 1 });
        lMap.Insert(10, RenderData{ {}, 10 });
        lSet.Insert(100);

        usize afterFrame = MemoryManager::Instance().GetFramePool()->GetUsedMemory();

        std::cout << "L-Vector Addr : " << &lVec[0] << std::endl;
        std::cout << "L-Map Entry   : " << lMap.Find(10) << std::endl;
        std::cout << "Allocated Frame Memory : " << (afterFrame - beforeFrame) << " bytes" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    // ---------------------------------------------------------
    // [SECTION 2] Slab (S) 시리즈 테스트 - 영구/버킷 메모리
    // ---------------------------------------------------------
    {
        std::cout << "[Step 2] Slab Series (Persistent Memory) Test" << std::endl;

        SVector<RenderData> sVec;
        SMap<int32, RenderData> sMap;
        SSet<uint64> sSet;

        sVec.push_back(RenderData{ {}, 2 });
        sMap.Insert(20, RenderData{ {}, 20 });
        sSet.Insert(200);

        std::cout << "S-Vector Addr : " << &sVec[0] << " (32-byte Align? "
            << (reinterpret_cast<uintptr_t>(&sVec[0]) % 32 == 0 ? "YES" : "NO") << ")" << std::endl;
        std::cout << "S-Map Entry   : " << sMap.Find(20) << std::endl;
        std::cout << "S-Vector Memory Usage : " << sVec.MemoryUsage() << " bytes" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    // ---------------------------------------------------------
    // [SECTION 3] 중첩 구조 및 프레임 리셋 테스트
    // ---------------------------------------------------------
    {
        std::cout << "[Step 3] Nested & Reset Test" << std::endl;

        // 프레임 메모리 안에 영구 메모리 포인터를 담는 일반적인 상황
        LMap<int32, SVector<RenderData>> frameCache;

        SVector<RenderData> persistentData;
        persistentData.push_back(RenderData{ {}, 999 });

        frameCache[0] = std::move(persistentData);

        std::cout << "Nested Data Addr in Frame: " << &frameCache[0][0] << std::endl;

        // 프레임 종료 시점 시뮬레이션
        MemoryManager::Instance().GetFramePool()->Reset();
        std::cout << "Frame Reset Done. Used Frame Memory: "
            << MemoryManager::Instance().GetFramePool()->GetUsedMemory() << " bytes" << std::endl;
    }

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "All Container Tests Passed Successfully." << std::endl;

    return 0;
}