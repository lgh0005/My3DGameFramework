#include "pch.h"
#include "Utils/MemoryUtils.h"
using namespace MGF3D;

// 32바이트 정렬이 필요한 특수 구조체 (엔진 최소 16보다 큰 경우)
struct alignas(32) AVXData
{
    float values[8];
};

int main()
{
    // 1. 1KB 크기의 스택 풀 준비 (버퍼 자체도 정렬 보장)
    alignas(16) byte buffer[1024];
    StackMemoryPool framePool(buffer, sizeof(buffer));

    std::cout << "--- Engine Memory Policy Test ---" << std::endl;
    std::cout << "MIN_ALIGNMENT      : " << MIN_ALIGNMENT << std::endl;
    std::cout << "Pool Start Address : " << (void*)buffer << std::endl;
    std::cout << "---------------------------------" << std::endl;

    {
        // 2. 소형 타입 테스트 (int32: 4바이트 정렬 -> 16바이트로 상향되어야 함)
        std::vector<int32, StackAllocator<int32>> intVec{ StackAllocator<int32>(&framePool) };
        intVec.push_back(10);

        std::cout << "[Step 1] int32 Allocation" << std::endl;
        std::cout << "Addr: " << &intVec[0] << " (Must end in 0, 16, 32...)" << std::endl;

        // 3. 라이브러리 타입 테스트 (vec3: 4~8바이트 정렬 -> 16바이트로 상향되어야 함)
        std::vector<vec3, StackAllocator<vec3>> vec3Vec{ StackAllocator<vec3>(&framePool) };
        vec3Vec.push_back({ 1.0f, 1.0f, 1.0f });

        std::cout << "\n[Step 2] vec3 Allocation" << std::endl;
        std::cout << "Addr: " << &vec3Vec[0] << " (Must end in 0, 16, 32...)" << std::endl;

        // 4. 대형 정렬 타입 테스트 (AVXData: 32바이트 정렬 -> 엔진 기준보다 크므로 32 유지)
        std::vector<AVXData, StackAllocator<AVXData>> avxVec{ StackAllocator<AVXData>(&framePool) };
        avxVec.push_back(AVXData());

        std::cout << "\n[Step 3] AVXData (32-byte align) Allocation" << std::endl;
        std::cout << "Addr: " << &avxVec[0] << " (Must be multiple of 32)" << std::endl;

        // 5. 힙 폴백 테스트 (1KB 초과 할당)
        std::cout << "\n[Step 4] Heap Fallback Test" << std::endl;
        std::vector<byte, StackAllocator<byte>> largeVec{ StackAllocator<byte>(&framePool) };

        // 풀 크기를 확실히 넘기기 위해 2048바이트 할당
        for (int i = 0; i < 2048; ++i) largeVec.push_back(0);

        bool isInside = framePool.IsInPool(&largeVec[0]);
        std::cout << "Large Data Addr: " << (void*)&largeVec[0] << std::endl;
        std::cout << "Is in Stack Pool? : " << (isInside ? "YES" : "NO (Heap Allocated)") << std::endl;
    }

    std::cout << "---------------------------------" << std::endl;
    std::cout << "Final Pool Offset: " << framePool.GetUsedMemory() << std::endl;
    framePool.Reset();
    std::cout << "Pool Reset Done." << std::endl;

    return 0;
}

//#include "pch.h"
//using namespace MGF3D;
//
//int main()
//{
//    // 1. 테스트를 위한 1KB 스택 풀 생성
//    byte buffer[1024];
//    StackMemoryPool framePool(buffer, sizeof(buffer));
//
//    std::cout << "--- Memory Pool Test Start ---" << std::endl;
//    std::cout << "Pool Start Address : " << (void*)buffer << std::endl;
//    std::cout << "Initial Offset     : " << framePool.GetUsedMemory() << std::endl;
//    std::cout << "-----------------------------" << std::endl;
//
//    {
//        // 2. 중괄호 {}를 사용하여 함수 선언이 아님을 명시
//        std::vector<int32, StackAllocator<int32>> myVec{ StackAllocator<int32>(&framePool) };
//
//        std::cout << "[Step 1] Pushing elements into Stack..." << std::endl;
//        for (int32 i = 0; i < 10; ++i)
//        {
//            myVec.push_back(i);
//            std::cout << "Element [" << i << "] Addr: " << &myVec[i]
//                << " | Offset: " << framePool.GetUsedMemory() << std::endl;
//        }
//
//        // 3. 정렬 확인용 (vec3)
//        std::vector<vec3, StackAllocator<vec3>> vec3Test{ StackAllocator<vec3>(&framePool) };
//        vec3Test.push_back({ 1.0f, 1.0f, 1.0f });
//
//        std::cout << "\n[Step 2] Alignment Check (vec3)" << std::endl;
//        std::cout << "vec3 Addr: " << &vec3Test[0]
//            << " (Must be multiple of 16 if alignment=16)" << std::endl;
//        std::cout << "Current Offset: " << framePool.GetUsedMemory() << std::endl;
//
//        // 4. 힙 폴백 테스트
//        std::cout << "\n[Step 3] Heap Fallback Test (Allocating 2KB)..." << std::endl;
//        std::vector<int32, StackAllocator<int32>> largeVec{ StackAllocator<int32>(&framePool) };
//
//        for (int32 i = 0; i < 512; ++i)
//        {
//            largeVec.push_back(i);
//        }
//
//        bool isInside = framePool.IsInPool(&largeVec[0]);
//        std::cout << "Large Vector Data Pointer : " << &largeVec[0] << std::endl;
//        std::cout << "Is Inside Stack Pool?     : " << (isInside ? "YES" : "NO (Heap Allocated)") << std::endl;
//    }
//
//    // 5. 정리
//    std::cout << "-----------------------------" << std::endl;
//    std::cout << "Vectors destroyed. Final Pool Offset: " << framePool.GetUsedMemory() << std::endl;
//
//    framePool.Reset();
//    std::cout << "Pool Reset. Current Offset: " << framePool.GetUsedMemory() << std::endl;
//
//    return 0;
//}