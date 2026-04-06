#pragma once

class WindowManager
{
	DECLARE_SINGLE(WindowManager)
	DECLARE_NONINSTANTIABLE(WindowManager)

public:
	bool Init(int32 width, int32 height, const std::string& title);
	GLFWwindow* GetWindow() { return m_window; }
	WindowManager* GetWindowUserPtr();
	void DestroyWindow();

	int32 GetWindowWidth() const { return m_width; }
	int32 GetWindowHeight() const { return m_height; }

	bool IsInconified() { return m_isIconified; }
	void SetIconified(bool iconified) { m_isIconified = iconified; }

private:
	void RegisterStaticEventCallbacks();
	static void HandleFramebufferSizeChange(GLFWwindow* window, int32 width, int32 height);
	static void HandleWindowIconified(GLFWwindow* window, int32 iconified);

private:
	GLFWwindow* m_window	{ nullptr };
	bool m_isIconified		{ false };
	int32 m_width			{ 0 };
	int32 m_height			{ 0 };
};