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

    Context& GetContext();

private:
	Game() = default;
    GLFWwindow* m_window   = nullptr;
    ContextUPtr m_context  = nullptr;

/*=============================//
//   default callback members  //
//=============================*/
#pragma region DEFAULT_CALLBACK_MEMBERS
// TODO : 이후에 Window 매니저 클래스가 수행하도록 수정해야 한다.
private:

    // Windows
    void HandleFramebufferSizeChange(int32 width, int32 height);
    void HandleWindowIconified(int32 iconified);

    static void OnFramebufferSizeChange(GLFWwindow* window, int32 width, int32 height);
    static void OnWindowIconified(GLFWwindow* window, int32 iconified);

    // Inputs
    void HandleKeyEvent(int32 key, int32 scancode, int32 action, int32 mods);
    void HandleCursorMove(double x, double y);
    void HandleMouseButton(int32 button, int32 action, int32 mod);

    static void OnKeyEvent(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods);
    static void OnCursorPos(GLFWwindow* window, double x, double y);
    static void OnMouseButton(GLFWwindow* window, int32 button, int32 action, int32 mod);
#pragma endregion
};