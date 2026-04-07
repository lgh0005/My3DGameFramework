#include "pch.h"
#include "Managers/WindowManager.h"
#include "Managers/TimeManager.h"
#include "Managers/InputManager.h"

#include "Input/Devices/MGFKeyboardDevice.h"
#include "Input/Devices/MGFMouseDevice.h"
#include <GLFW/glfw3.h>

using namespace MGF3D;

int main()
{
	MGF_LOG_INIT();

	// 2. 윈도우 매니저 초기화 (800x600, VSync 끄기(0)로 테스트하여 최대 FPS 확인)
	if (!MGF_WINDOW.Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, 0))
	{
		MGF_LOG_FATAL("Failed to initialize WindowManager.");
		return -1;
	}

	// 3. 타임 매니저
	MGF_TIME.Init();

	// 4. 키 매핑 (초기화 시 1회)
	MGF_INPUT.Init(MGF_WINDOW.GetNativeHandle());
	auto* kbd = MGF_INPUT.GetDevice<MGFKeyboardDevice>("Keyboard"); if (!kbd) return -1;
	kbd->MapKey("Jump", GLFW_KEY_SPACE);
	kbd->MapKey("Exit", GLFW_KEY_ESCAPE);

	auto* mouse = MGF_INPUT.GetDevice<MGFMouseDevice>("Mouse"); if (!mouse) return -1;
	mouse->MapButton("Fire", GLFW_MOUSE_BUTTON_LEFT);

	// 4. 메인 엔진 루프
	while (!MGF_WINDOW.ShouldClose())
	{
		MGF_TIME.Update();

		MGF_WINDOW.Update();

		// 2. 액션 기반 로직 처리
		MGF_INPUT.Update(MGF_WINDOW.GetNativeHandle());

		if (kbd->GetButtonDown("Jump")) 
		{
			MGF_LOG_INFO("Action: Jump!");
		}

		if (mouse->GetButtonDown("Fire")) 
		{
			MGF_LOG_INFO("Action: Fire!");
		}

		MGF_TIME.FixedUpdate();

		MGF_WINDOW.SwapWindowBuffers();
	}

	MGF_WINDOW.Shutdown();
	MGF_LOG_CLEAR();
}