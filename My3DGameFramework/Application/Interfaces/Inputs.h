#pragma once
#include "Managers/InputManager.h"

namespace MGF3D
{
	class Inputs
	{
		MGF_DECLARE_UTILITIES(Inputs)

	/*==================================//
	//    Input Mapping API             //
	//==================================*/
	public:
		static void MapKeyboard(const String& action, int32 key);
		static void MapMouse(const String& action, int32 button);

	/*==================================//
	//    Input Polling API             //
	//==================================*/
	public:
		// 키/버튼이 눌려 있는 상태인지 확인
		static bool GetButton(const String& action);

		// 키/버튼이 방금 눌렸는지 확인 (Down)
		static bool GetButtonDown(const String& action);

		// 키/버튼이 방금 떼졌는지 확인 (Up)
		static bool GetButtonUp(const String& action);

		// 마우스 현재 위치 가져오기
		static vec2 GetMousePosition();

		// 마우스 변위 벡터 가져오기
		static vec2 GetMouseDelta();
	};
}