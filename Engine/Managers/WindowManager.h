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
	void Init(GLFWwindow* handle);
	GLFWwindow* GetHandle() { return m_handle; }
	bool IsInconified() { return m_isIconified; }

	// TEMP : 코드를 옮겨둠.
	void HandleFramebufferSizeChange(Context& context, int32 width, int32 height);
	void HandleWindowIconified(int32 iconified);

private:
	GLFWwindow* m_handle	{ nullptr };
	bool m_isIconified		{ false };
};