#include "pch.h"
#include "Bootstrapper/Bootstrapper.h"
#include "Debug/MemoryProfiler.h"

#include "Managers/TimeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/GLFWManager.h"
#include "Managers/TaskManager.h"
#include "Managers/PathManager.h"
#include "Managers/InputManager.h"

using namespace MGF3D;

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