#include "pch.h"
#include "Managers/MemoryManager.h"
#include "Memory/SlabAllocator.h"
#include "Memory/LinearAllocator.h"
#include <unordered_map>

using namespace MGF3D;

// 테스트용 구조체 (32바이트 정렬 확인용)
struct alignas(32) RenderData
{
    float matrix[16]; // 64 bytes
};

int main()
{
    // 1. 엔진 메모리 사령부 초기화 (내부에서 68MB 즉시 확보)
    MemoryManager::Instance();

    std::cout << "=== MGF3D Memory System Integration Test ===" << std::endl;
    std::cout << "Slab Buckets: 36MB, Linear(Scratch): 32MB Allocated." << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    {
        // [테스트 1] LinearAllocator (프레임 임시 메모리)
        auto* scratchPool = MemoryManager::Instance().GetFramePool();
        std::vector<int32, LinearAllocator<int32>> tempVec{ LinearAllocator<int32>(scratchPool) };

        std::cout << "[Step 1] Linear(Scratch) Allocation" << std::endl;
        for (int i = 0; i < 100; ++i) tempVec.push_back(i);

        std::cout << "Vector Data Addr : " << &tempVec[0] << std::endl;
        std::cout << "Used Scratch     : " << scratchPool->GetUsedMemory() << " bytes" << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
    }

    {
        // [테스트 2] SlabAllocator (Slab 버킷 시스템)
        std::cout << "[Step 2] Heap(Slab) Bucket Allocation" << std::endl;
        std::vector<RenderData, SlabAllocator<RenderData>> persistentVec;
        persistentVec.push_back(RenderData());

        std::cout << "RenderData Size  : " << sizeof(RenderData) << " bytes" << std::endl;
        std::cout << "RenderData Addr  : " << &persistentVec[0] << " (32-byte Aligned?)" << std::endl;

        if (reinterpret_cast<uintptr>(&persistentVec[0]) % 32 == 0)
            std::cout << "Alignment Check  : SUCCESS" << std::endl;
        else
            std::cout << "Alignment Check  : FAILED" << std::endl;

        std::cout << "--------------------------------------------" << std::endl;
    }

    {
        // [테스트 3] 대규모 할당 및 폴백
        std::cout << "[Step 3] System Heap Fallback Test (> 4KB)" << std::endl;
        std::vector<byte, SlabAllocator<byte>> largeVec;
        largeVec.resize(8192);

        std::cout << "Large Data Addr  : " << (void*)&largeVec[0] << " (Outside of MGF3D Pre-allocated range)" << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
    }

    {
        // [테스트 4] 중첩 컨테이너 테스트 (unordered_map<int, vector<RenderData>>)
        std::cout << "[Step 4] Nested Container (Map of Vectors) Test" << std::endl;

        // 1. 내부 벡터 타입 정의 (우리 SlabAllocator 사용)
        using InnerVector = std::vector<RenderData, SlabAllocator<RenderData>>;

        // 2. 맵 타입 정의 (Key: int, Value: InnerVector, Allocator: SlabAllocator)
        // 맵의 노드(pair<const int, InnerVector>)도 우리 슬랩 버킷에서 할당됩니다.
        using TestMap = std::unordered_map<
            int32,
            InnerVector,
            std::hash<int32>,
            std::equal_to<int32>,
            SlabAllocator<std::pair<const int32, InnerVector>>
        >;

        TestMap myMap;

        // 데이터 삽입
        for (int i = 0; i < 5; ++i)
        {
            InnerVector vec;
            vec.push_back(RenderData()); // 64바이트 슬랩 할당
            myMap[i] = std::move(vec);   // 맵 노드 슬랩 할당
        }

        std::cout << "Map Size          : " << myMap.size() << " elements" << std::endl;

        // 주소 확인 (둘 다 우리 m_baseBuffer 범위 내에 있어야 함)
        auto it = myMap.find(0);
        if (it != myMap.end())
        {
            std::cout << "Map Node Addr     : " << &(*it) << " (In Slab Bucket)" << std::endl;
            std::cout << "Vector Data Addr  : " << &(it->second[0]) << " (In Slab Bucket)" << std::endl;
        }
        std::cout << "--------------------------------------------" << std::endl;
    }

    // 2. 프레임 종료 시 리셋 (Linear 메모리만 비워짐)
    MemoryManager::Instance().GetFramePool()->Reset();
    std::cout << "Frame End: Linear Pool Reset Done." << std::endl;

    return 0;
}