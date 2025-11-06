#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Context)
#pragma endregion

CLASS_PTR(Game)
class Game
{
public:
	static GameUPtr Create();
	bool Init();
	void Update();
	void Shutdown();
    ~Game();

private:
	Game() = default;
	GLFWwindow* m_window   = nullptr;
    ContextUPtr m_context  = nullptr;
};

/*======================//
//   default callbacks  //
//======================*/
#pragma region DEFAULT_CALLBACKS

// 윈도우 크기 변경 콜백
inline void OnFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
	SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
	glViewport(0, 0, width, height);
}

inline void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

#pragma endregion