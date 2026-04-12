#include "CorePch.h"
#include "Application.h"

#pragma region MODULES
#include "CoreModule.h"
#pragma endregion

#pragma region MANAGERS
#include "Managers/InputManager.h"
#include "Managers/WindowManager.h"
#include "Managers/TimeManager.h"
#include "Managers/SceneManager.h"
#include "Managers/TypeManager.h"
#pragma endregion

#pragma region TEMP
#include "Input/Devices/MGFKeyboardDevice.h"
#include "Input/Devices/MGFMouseDevice.h"
#pragma endregion

namespace MGF3D
{
	void Application::RegisterTypes()
	{
		CoreModule::OnRegisterTypes();
	}

	bool Application::Init()
	{
		if (!CoreModule::OnInit()) return false;

		// 0-1. [DEBUG] 디바이스 가져오기 및 키 매핑
		{
			auto* kbd = MGF_INPUT.GetDevice<MGFKeyboardDevice>(); if (!kbd) return -1;
			kbd->MapKey("Jump", GLFW_KEY_SPACE);

			auto* mouse = MGF_INPUT.GetDevice<MGFMouseDevice>(); if (!mouse) return -1;
			mouse->MapButton("Fire", GLFW_MOUSE_BUTTON_LEFT);
		}

		return true;
	}

	void Application::Run()
	{
		while (!MGF_WINDOW.ShouldClose())
		{
			MGF_TIME.Update();
			MGF_WINDOW.Update();

			// 2. [DEBUG] 액션 기반 로직 처리
			{
				MGF_INPUT.Update(MGF_WINDOW.GetNativeHandle());
				if (MGF_INPUT.GetDevice<MGFKeyboardDevice>()->GetButtonDown("Jump")) MGF_LOG_INFO("Action: Jump!");
				if (MGF_INPUT.GetDevice<MGFMouseDevice>()->GetButtonDown("Fire"))  MGF_LOG_INFO("Action: Fire!");
			}

			MGF_SCENE.Update();

			MGF_TIME.FixedUpdate();
			MGF_WINDOW.SwapWindowBuffers();
		}
	}

	bool Application::Shutdown()
	{
		// TODO : 파괴는 생성의 역순

		// 0. 코어 모듈 종료
		if (!CoreModule::OnShutdown()) return false;

		return true;
	}
}