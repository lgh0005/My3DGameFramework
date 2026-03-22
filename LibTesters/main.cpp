#include "pch.h"
#include "Bootstrapper/Bootstrapper.h"
#include "Debug/MemoryProfiler.h"
#include "Debug/VRAMProfiler.h"
#include "Pointer/RefCount.h"

#include "Managers/TimeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/GLFWManager.h"
#include "Managers/TaskManager.h"
#include "Managers/PathManager.h"
#include "Managers/InputManager.h"
#include "Managers/VRAMManager.h"

using namespace MGF3D;

class TestActor : public RefCount
{
public:
    int payload[512]; // 4KB (1024 * 4 bytes)
    TestActor() { MGF_LOG_INFO("TestActor Created! (Memory Allocated in Slab)"); }
    ~TestActor() { MGF_LOG_INFO("TestActor Destroyed! (Memory Freed to Slab)"); }
};

int main()
{
    if (!Bootstrapper::Init())
    {
        MGF_LOG_FATAL("Engine Bootstrapping Failed!");
        return -1;
    }

    MGF_LOG_INFO("=== MGF3D Integrated System & Window Test ===");

    // [체크포인트 A] 엔진 초기화 직후 메모리 상태
    MGF_LOG_WARN(">> [Checkpoint A] Engine Base Memory (After Init)");
    MGF_MEMORY_PROFILE_CAPTURE();
    MGF_MEMORY_LOG_STATS();

    /*=================================================//
    // [테스트 1] 입력 매핑 및 콜백 바인딩 (초기화 단계)  //
    //=================================================*/
    // 1. ESC 키를 누르면 창이 닫히도록 이벤트 바인딩 (Edge Trigger)
    MGF_INPUT.MapAction("QuitGame", KeyCode::Escape);
    MGF_INPUT.BindAction("QuitGame", InputEvent::Pressed, []()
    {
        MGF_LOG_INFO("ESC Pressed! Requesting window shutdown...");
        glfwSetWindowShouldClose(MGF_WINDOW.GetNativeWindow(), true);
    });

    // 2. 마우스 좌클릭 시 공격 로그 출력
    MGF_INPUT.MapMouseAction("Fire", MouseCode::Left);
    MGF_INPUT.BindAction("Fire", InputEvent::Pressed, []()
    {
        MGF_LOG_INFO("Pew Pew! Left Mouse Button Pressed!");
    });

    // 3. W키 매핑 (이건 아래 폴링 루프에서 지속적으로 검사할 예정입니다)
    MGF_INPUT.MapAction("MoveForward", KeyCode::W);

    // [체크포인트 B] 윈도우 생성 및 루프 진입 전
    // (이 시점에서 GLFW가 내부적으로 창 객체, 모니터 정보 등을 Slab에 할당했을 것입니다)
    MGF_LOG_WARN(">> [Checkpoint B] After Window & Input Mapping");
    MGF_MEMORY_PROFILE_CAPTURE();
    MGF_MEMORY_LOG_STATS();

    /*=================================================//
    // [테스트 2] 커스텀 스마트 포인터 메모리 테스트      //
    //=================================================*/
    MGF_LOG_WARN(">> [Memory Test] 1. Before UniquePtr Scope");
    MGF_MEMORY_PROFILE_CAPTURE();
    MGF_MEMORY_LOG_STATS();

    {
        // 중괄호 블록 시작: 여기서 MakeUnique를 호출합니다!
        MGF_LOG_INFO("--- Entering Scope ---");

        // 우리가 만든 엔진 전용 팩토리 함수를 사용! (Slab 할당)
        auto myActor = MakeUnique<TestActor>();

        MGF_LOG_WARN(">> [Memory Test] 2. Inside Scope (Actor is Alive!)");
        MGF_MEMORY_PROFILE_CAPTURE();
        MGF_MEMORY_LOG_STATS(); // 여기서 Slab 할당량이 정확히 4KB(4096 bytes) 증가해야 합니다!

        MGF_LOG_INFO("--- Exiting Scope ---");
    } // 중괄호 블록 끝: myActor가 죽으면서 우리가 만든 Deleter가 MemoryManager.Deallocate를 자동 호출합니다!

    MGF_LOG_WARN(">> [Memory Test] 3. After Scope (Actor should be Destroyed)");
    MGF_MEMORY_PROFILE_CAPTURE();
    MGF_MEMORY_LOG_STATS();

    /*=================================================//
    // [테스트 3] 커스텀 SharedPtr 메모리 테스트         //
    //=================================================*/
    MGF_LOG_WARN(">> [Memory Test] 4. Before SharedPtr Scope");
    MGF_MEMORY_PROFILE_CAPTURE();
    MGF_MEMORY_LOG_STATS();

    {
        MGF_LOG_INFO("--- Entering SharedPtr Scope ---");

        // 1. 첫 번째 SharedPtr 생성 (여기서 할당 발생)
        auto sharedActor1 = MakeShared<TestActor>();

        MGF_LOG_WARN(">> [Memory Test] 5. Inside Scope (sharedActor1 is Alive!)");
        MGF_LOG_INFO("    Current RefCount: {}", sharedActor1->GetRefCount());
        MGF_MEMORY_PROFILE_CAPTURE();
        MGF_MEMORY_LOG_STATS();

        // 2. 참조 카운팅 테스트 (내부 스코프)
        {
            MGF_LOG_INFO("    --- Entering Nested Scope ---");
            auto sharedActor2 = sharedActor1; // 복사 발생! (메모리 할당은 없고 카운트만 증가)
            MGF_LOG_INFO("    sharedActor2 created! Current RefCount: {}", sharedActor1->GetRefCount());
            MGF_LOG_INFO("    --- Exiting Nested Scope ---");
        } // sharedActor2 소멸 (카운트 감소, 하지만 1이 남았으므로 파괴 안 됨!)

        MGF_LOG_INFO("    After Nested Scope! Current RefCount: {}", sharedActor1->GetRefCount());
        MGF_LOG_INFO("--- Exiting SharedPtr Scope ---");

    } // 여기서 최종적으로 RefCount가 0이 되며 TestActor가 파괴되고 메모리 반환!

    MGF_LOG_WARN(">> [Memory Test] 6. After SharedPtr Scope (Memory should be restored)");
    MGF_MEMORY_PROFILE_CAPTURE();
    MGF_MEMORY_LOG_STATS();

    /*=================================================//
    // [테스트 4] VRAM 직접 할당 및 프로파일링 테스트     //
    //=================================================*/
    MGF_LOG_WARN(">> [VRAM Test] 1. Before VRAM Allocation");
    MGF_VRAM_PROFILE_CAPTURE();
    MGF_VRAM_LOG_STATS();

    {
        MGF_LOG_INFO("--- Allocating 16MB from Static VRAM Pool ---");
        // 16MB 크기, 256 바이트 정렬로 Static Pool에 할당 요청
        VRAMAllocation testVramAlloc = VRAMManager::Instance().Allocate(VRAMAllocation::PoolType::Static, 16 * 1024 * 1024, 256);

        MGF_LOG_WARN(">> [VRAM Test] 2. After Allocation (Static Pool usage should increase)");
        MGF_VRAM_PROFILE_CAPTURE();
        MGF_VRAM_LOG_STATS();

        MGF_LOG_INFO("--- Deallocating 16MB from Static VRAM Pool ---");
        VRAMManager::Instance().Deallocate(testVramAlloc);
    }

    MGF_LOG_WARN(">> [VRAM Test] 3. After Deallocation (VRAM should be restored)");
    MGF_VRAM_PROFILE_CAPTURE();
    MGF_VRAM_LOG_STATS();

    /*=================================================//
    //                 메인 게임 루프                   //
    //=================================================*/
    while (!MGF_WINDOW.ShouldClose())
    {
        // 0. 시간 업데이트
        MGF_TIME.Update();

        // 1. OS 이벤트 처리
        MGF_GLFW.Update();

        // 2. 인풋 시스템 상태 동기화
        MGF_INPUT.Update(MGF_WINDOW.GetNativeWindow());

        // 2-1. 폴링(Polling) 방식 테스트: W키를 꾹 누르고 있을 때 (Level Trigger)
        //if (MGF_INPUT.GetButton("MoveForward"))
        //    MGF_LOG_INFO("Moving Forward... (W key is held down)");

        // 2-2. 이번 프레임에 막 눌렸을 때만 반응하는지 테스트
        if (MGF_INPUT.GetButtonDown("MoveForward"))
            MGF_LOG_INFO("W key JUST PRESSED this frame!");

        // 2. 엔진 시스템 업데이트
        MGF_TASK.Update();

        // 3. 렌더링 결과 출력 (SwapBuffers)
        MGF_WINDOW.Update();
    }

    // [체크포인트 C] 엔진 종료 직전 (메모리가 모두 반환되었는지 확인 가능)
    MGF_LOG_WARN(">> [Checkpoint C] Final Memory State before Exit");
    MGF_MEMORY_PROFILE_CAPTURE();
    MGF_MEMORY_LOG_STATS();

    Bootstrapper::Shutdown();

    return 0;
}