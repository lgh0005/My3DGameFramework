#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Context)
#pragma endregion

// TODO : 필요하다면 여러 이벤트를 받아서 처리할 수 있는 구조로
// 수정 필요
class WindowManager
{
	DECLARE_SINGLE(WindowManager)

public:
	bool Init();
	GLFWwindow* GetWindow() { return m_window; }
	WindowManager* GetWindowUserPtr();
	void DestroyWindow();

	// TODO : 확정적으로 있어야 할 콜백 함수들을 골라서 
	// TEMP : miscellaneous
	bool IsInconified() { return m_isIconified; }

private:
	// 등록할 수 있도록 수정 필요
	void RegisterStaticEventCallbacks();

	// TEMP : static window event callback
	static void HandleFramebufferSizeChange(GLFWwindow* window, int32 width, int32 height);
	static void HandleWindowIconified(GLFWwindow* window, int32 iconified);

private:
	GLFWwindow* m_window	{ nullptr };
	bool m_isIconified		{ false };
};