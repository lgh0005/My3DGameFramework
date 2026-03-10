#include "pch.h"
#include "Bootstrapper/Bootstrapper.h"

#include "Managers/TimeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/GLFWManager.h"
#include "Managers/TaskManager.h"
#include "Managers/PathManager.h"
using namespace MGF3D;

int main()
{
    if (!Bootstrapper::Init())
    {
        MGF_LOG_FATAL("Engine Bootstrapping Failed!");
        return -1;
    }

    MGF_LOG_INFO("=== MGF3D Integrated System & Window Test ===");

    while (!MGF_WINDOW.ShouldClose())
    {
        // 0. 시간 업데이트
        MGF_TIME.Update();

        // 1. OS 이벤트 처리
        MGF_GLFW.Update();

        // 2. 엔진 시스템 업데이트
        MGF_TASK.Update();

        // 3. 렌더링 결과 출력 (SwapBuffers)
        MGF_WINDOW.Update();
    }

    Bootstrapper::Shutdown();

    return 0;
}