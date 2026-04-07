#include "CorePch.h"
#include "MGFInputInterface.h"
#include "Managers/InputManager.h"
#include "Devices/MGFKeyboardDevice.h"
#include "Devices/MGFMouseDevice.h"
#include <GLFW/glfw3.h>

namespace MGF3D
{
	void MGFInputInterface::Install(GLFWwindow* window)
	{
		if (window == nullptr) return;

		// Keyboard
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetCharCallback(window, CharCallback);

		// Mouse
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwSetCursorPosCallback(window, CursorPosCallback);
		glfwSetCursorEnterCallback(window, CursorEnterCallback);
		glfwSetScrollCallback(window, ScrollCallback);

		// Joystick
		glfwSetJoystickCallback(JoystickCallback);

		// System
		glfwSetDropCallback(window, DropCallback);
		glfwSetWindowFocusCallback(window, WindowFocusCallback);
	}

	/*==============================//
	//      Keyboard Callbacks      //
	//==============================*/
	void MGFInputInterface::KeyCallback(GLFWwindow*, int32 key, int32 scancode, int32 action, int32 mods)
	{
		auto* keyboardDevice = MGF_INPUT.GetDevice<MGFKeyboardDevice>();
		if (keyboardDevice) keyboardDevice->OnKey(key, action);
	}

	void MGFInputInterface::CharCallback(GLFWwindow*, uint32 codepoint)
	{
		// 텍스트 입력 처리가 필요한 경우 여기에 구현 (예: UI 텍스트 박스)
		// 현재는 로그 혹은 특정 장치로 전파하지 않음
	}

	/*==============================//
	//       Mouse Callbacks        //
	//==============================*/
	void MGFInputInterface::MouseButtonCallback(GLFWwindow*, int32 button, int32 action, int32 mods)
	{
		auto* mouseDevice = MGF_INPUT.GetDevice<MGFMouseDevice>();
		if (mouseDevice) mouseDevice->OnMouseButton(button, action);
	}

	void MGFInputInterface::CursorPosCallback(GLFWwindow*, double xpos, double ypos)
	{
		auto* mouseDevice = MGF_INPUT.GetDevice<MGFMouseDevice>();
		if (mouseDevice) mouseDevice->OnCursorPos(xpos, ypos);
	}

	void MGFInputInterface::CursorEnterCallback(GLFWwindow*, int32 entered)
	{
		// 마우스가 윈도우 영역에 들어오거나 나갈 때 처리
		// 예: entered == GLFW_TRUE 이면 활성화
	}

	void MGFInputInterface::ScrollCallback(GLFWwindow*, double xoffset, double yoffset)
	{
		// 마우스 휠 입력을 처리 (MouseDevice에 OnScroll 추가 필요)
	}

	/*==============================//
	//      Joystick Callbacks      //
	//==============================*/
	void MGFInputInterface::JoystickCallback(int32 jid, int32 event)
	{
		switch (event)
		{
		case GLFW_CONNECTED:
			MGF_LOG_INFO("Joystick connected: ID {}", jid);
			// InputManager에 JoystickDevice 동적 등록 로직 추가 가능
			break;

		case GLFW_DISCONNECTED:
			MGF_LOG_INFO("Joystick disconnected: ID {}", jid);
			// InputManager에서 해당 JoystickDevice 제거 로직 추가 가능
		}
	}

	/*==============================//
	//       System Callbacks       //
	//==============================*/
	void MGFInputInterface::DropCallback(GLFWwindow*, int32 count, const char** paths)
	{
		for (int32 i = 0; i < count; ++i)
		{
			MGF_LOG_INFO("File dropped: {}", paths[i]);
			// 에셋 로더나 드래그 앤 드롭 시스템으로 경로 전달
		}
	}

	void MGFInputInterface::WindowFocusCallback(GLFWwindow*, int32 focused)
	{
		if (focused == GLFW_FALSE)
		{
			// 창이 포커스를 잃었을 때 모든 입력 상태를 초기화하는 로직 등을 수행할 수 있음
		}
	}
}