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

void PrintTypeTreeDebug()
{
	MGF_LOG_INFO("=== [Debug] Device Type Tree Hierarchy ===");

	MGFTypeTree* deviceTree = MGF_TYPE.GetTree("Device");
	if (deviceTree)
	{
		// MAX_TYPE_COUNT 범위 내에서 유효한 타입을 순회하며 정보 출력
		for (int16 i = 0; i < MAX_TYPE_COUNT; ++i)
		{
			const MGFType* type = deviceTree->GetType(i);

			// GetType 내부에서 범위 검사를 하지만, 빈 슬롯에 대한 IsValid 검사를 추가로 수행
			if (type && type->IsValid())
			{
				MGF_LOG_INFO("Index: {0} | Name: {1} | ParentIdx: {2} | Depth: {3}",
					type->selfIndex,
					type->typeName,
					type->parentIndex,
					type->depth);
			}
		}
	}
	else
	{
		MGF_LOG_ERROR("Device tree not found.");
	}

	MGF_LOG_INFO("==========================================");
}

int main()
{
	// 1. 모듈 정적 초기화 (타입 등록 및 시스템 초기화)
	CoreModule::OnRegisterTypes();
	if (!CoreModule::OnInit()) return -1;

	// 2. 타입 계층 구조 디버그 확인
	PrintTypeTreeDebug();

	// 3. 디바이스 가져오기 및 키 매핑
	auto* kbd = MGF_INPUT.GetDevice<MGFKeyboardDevice>("Keyboard"); if (!kbd) return -1;
	kbd->MapKey("Jump", GLFW_KEY_SPACE);

	auto* mouse = MGF_INPUT.GetDevice<MGFMouseDevice>("Mouse"); if (!mouse) return -1;
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