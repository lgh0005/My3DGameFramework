#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Context)
#pragma endregion

// TODO : 나중에는 여러 이벤트를 받아서 처리할 수 있는 구조로
// 수정 필요
class InputManager
{
	DECLARE_SINGLE(InputManager)

public:
	void HandleKeyEvent(int32 key, int32 scancode, int32 action, int32 mods);
	void HandleCharEvent(uint32 ch);
	void HandleScroll(double xOffset, double yOffset);
	void HandleCursorMove(Context& context, double x, double y);
	void HandleMouseButton(Context& context, int32 button, int32 action, int32 mod);
};