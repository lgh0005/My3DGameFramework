#include "InputManager.h"
#include "EnginePch.h"
#include "InputManager.h"
#include "Graphics/Context.h"

void InputManager::HandleKeyEvent(int32 key, int32 scancode, int32 action, int32 mods)
{
	SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
		key, scancode,
		action == GLFW_PRESS ? "Pressed" :
		action == GLFW_RELEASE ? "Released" :
		action == GLFW_REPEAT ? "Repeat" : "Unknown",
		mods & GLFW_MOD_CONTROL ? "C" : "-",
		mods & GLFW_MOD_SHIFT ? "S" : "-",
		mods & GLFW_MOD_ALT ? "A" : "-");

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		// m_window 대신 WindowManager에서 핸들을 가져와 사용
		glfwSetWindowShouldClose(WINDOW.GetHandle(), true);
	}
}

void InputManager::HandleCursorMove(Context& context, double x, double y)
{
	context.MouseMove(x, y);
}

void InputManager::HandleMouseButton(Context& context, int32 button, int32 action, int32 mod)
{
	double x, y;
	glfwGetCursorPos(WINDOW.GetHandle(), &x, &y);
	context.MouseButton(button, action, x, y);
}
