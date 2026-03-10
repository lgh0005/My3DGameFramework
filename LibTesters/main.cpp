#include "pch.h"
#include "Bootstrapper/Bootstrapper.h"

#include "Managers/TimeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/GLFWManager.h"
#include "Managers/TaskManager.h"
#include "Managers/PathManager.h"
#include "Managers/InputManager.h"
#include <GLFW/glfw3.h> // TODO : 이거 매핑마다 계속 glfw3을 추가할 수 없으니 Utils에다가 키를 따로 우리만의 별칭으로 매핑 필요

using namespace MGF3D;

int main()
{
    if (!Bootstrapper::Init())
    {
        MGF_LOG_FATAL("Engine Bootstrapping Failed!");
        return -1;
    }

    MGF_LOG_INFO("=== MGF3D Integrated System & Window Test ===");

    /*=================================================//
    // [테스트 1] 입력 매핑 및 콜백 바인딩 (초기화 단계)  //
    //=================================================*/
    // 1. ESC 키를 누르면 창이 닫히도록 이벤트 바인딩 (Edge Trigger)
    MGF_INPUT.MapAction("QuitGame", GLFW_KEY_ESCAPE);
    MGF_INPUT.BindAction("QuitGame", InputEvent::Pressed, []()
    {
        MGF_LOG_INFO("ESC Pressed! Requesting window shutdown...");
        glfwSetWindowShouldClose(MGF_WINDOW.GetNativeWindow(), true);
    });

    // 2. 마우스 좌클릭 시 공격 로그 출력
    MGF_INPUT.MapMouseAction("Fire", GLFW_MOUSE_BUTTON_LEFT);
    MGF_INPUT.BindAction("Fire", InputEvent::Pressed, []()
    {
        MGF_LOG_INFO("Pew Pew! Left Mouse Button Pressed!");
    });

    // 3. W키 매핑 (이건 아래 폴링 루프에서 지속적으로 검사할 예정입니다)
    MGF_INPUT.MapAction("MoveForward", GLFW_KEY_W);

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

    Bootstrapper::Shutdown();

    return 0;
}