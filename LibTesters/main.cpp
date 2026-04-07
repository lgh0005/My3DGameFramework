#include "pch.h"
#include "Managers/WindowManager.h"
#include "Managers/TimeManager.h"
#include "Managers/InputManager.h"
#include "Managers/TypeManager.h"

#include "CoreModule.h"

#include "Identities/MGFTypeTree.h"
#include "Input/Devices/MGFKeyboardDevice.h"
#include "Input/Devices/MGFMouseDevice.h"
#include <GLFW/glfw3.h>
using namespace MGF3D;

int main()
{
	// 1. 모듈 정적 초기화 (타입 등록 및 시스템 초기화)
	CoreModule::OnRegisterTypes();
	if (!CoreModule::OnInit()) return -1;

	// 3. 디바이스 가져오기 및 키 매핑
	auto* kbd = MGF_INPUT.GetDevice<MGFKeyboardDevice>(); if (!kbd) return -1;
	kbd->MapKey("Jump", GLFW_KEY_SPACE);

	auto* mouse = MGF_INPUT.GetDevice<MGFMouseDevice>(); if (!mouse) return -1;
	mouse->MapButton("Fire", GLFW_MOUSE_BUTTON_LEFT);

	// 4. 메인 엔진 루프
	while (!MGF_WINDOW.ShouldClose())
	{
		MGF_TIME.Update();
		MGF_WINDOW.Update();

		// 2. 액션 기반 로직 처리
		MGF_INPUT.Update(MGF_WINDOW.GetNativeHandle());
		if (kbd->GetButtonDown("Jump")) MGF_LOG_INFO("Action: Jump!");
		if (mouse->GetButtonDown("Fire"))  MGF_LOG_INFO("Action: Fire!");

		MGF_TIME.FixedUpdate();
		MGF_WINDOW.SwapWindowBuffers();
	}

	// 6. 엔진 종료 (모듈 정적 종료 호출)
	CoreModule::OnShutdown();
}