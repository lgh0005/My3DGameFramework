#include "pch.h"
#include "Managers/MemoryManager.h"
#include "Containers/Slab/SVector.h"
#include "Containers/Slab/SMap.h"
#include "Containers/Slab/SSet.h"

using namespace MGF3D;

// 테스트용 구조체 (64바이트, 32바이트 정렬)
struct alignas(32) RenderData
{
    float matrix[16];

    // SSet 테스트를 위한 비교 연산자 (ID 기준)
    uint64 id;
    bool operator==(const RenderData& other) const { return id == other.id; }
};

// SSet 해시 지원을 위한 특수화 (ID 기준)
namespace std {
    template<> struct hash<RenderData> {
        size_t operator()(const RenderData& d) const { return hash<uint64>{}(d.id); }
    };
}

int main()
{
    // 1. 엔진 메모리 시스템 가동 (Singleton 초기화 및 Slab 버킷들 생성)
    MemoryManager::Instance();

    std::cout << "=== MGF3D Automated Memory System Test ===" << std::endl;
    std::cout << "Allocators will now automatically find their pools." << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    {
        // [테스트 1] LVector (Linear 할당 테스트)
        std::cout << "[Step 1] LVector Automatic Binding" << std::endl;
        // LVector는 프레임 단위 임시 메모리 할당용입니다.
        // LVector<int32> tempVec; 
        // ... (생략된 LVector 로직)
        std::cout << "--------------------------------------------" << std::endl;
    }

    {
        // [테스트 2] SVector (자동 Slab 버킷 바인딩)
        std::cout << "[Step 2] SVector Automatic Bucket Binding" << std::endl;

        MGF3D::SVector<RenderData> persistentVec;
        persistentVec.push_back(RenderData{ {}, 101 });

        std::cout << "SVector Data Addr : " << &persistentVec[0] << " (In 64-byte Bucket?)" << std::endl;

        if (reinterpret_cast<uintptr_t>(&persistentVec[0]) % 32 == 0)
            std::cout << "Alignment Check   : SUCCESS (32-byte Aligned)" << std::endl;

        std::cout << "Memory Usage      : " << persistentVec.MemoryUsage() << " bytes" << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
    }

    {
        // [테스트 3] SSet (우리가 만든 SSet 검증)
        std::cout << "[Step 3] SSet Read-Only Find & Slab Allocation" << std::endl;

        MGF3D::SSet<RenderData> assetSet;
        assetSet.Insert(RenderData{ {}, 202 });

        // 우리가 구현한 Read-only Find 호출
        const RenderData* found = assetSet.Find(RenderData{ {}, 202 });

        if (found)
        {
            std::cout << "SSet Node Addr    : " << found << " (Slab Bucket)" << std::endl;
            std::cout << "SSet Data ID      : " << found->id << std::endl;
        }
        std::cout << "--------------------------------------------" << std::endl;
    }

    {
        // [테스트 4] SMap (복합 Slab 자동 할당)
        std::cout << "[Step 4] SMap & SVector Nested Automation" << std::endl;

        // Map 노드(pair<int, SVector>)는 노드 크기에 맞는 버킷으로,
        // 내부 SVector의 데이터는 RenderData 크기(64) 버킷으로 각각 자동 분산됩니다.
        MGF3D::SMap<int32, MGF3D::SVector<RenderData>> myMap;

        for (int i = 0; i < 3; ++i)
        {
            MGF3D::SVector<RenderData> vec;
            vec.push_back(RenderData{ {}, (uint64)i });
            myMap[i] = std::move(vec);
        }

        auto it = myMap.find(0);
        if (it != myMap.end())
        {
            std::cout << "Map Node Addr     : " << &(*it) << " (Bucket for Map Node)" << std::endl;
            std::cout << "Vector Data Addr  : " << &(it->second[0]) << " (Bucket for 64-byte Data)" << std::endl;
        }
        std::cout << "--------------------------------------------" << std::endl;
    }

    // 2. 프레임 리셋 (Linear Allocator만 초기화됨, Slab은 유지)
    MemoryManager::Instance().GetFramePool()->Reset();
    std::cout << "Frame End: Linear Pool Reset Done." << std::endl;

    return 0;
}