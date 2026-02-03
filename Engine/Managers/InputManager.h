#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum class InputEvent
{
	Pressed = 0,
	Released = 1,
	MAX
};

struct InputAction
{
	std::string Name;
	std::vector<int32> MappedKeys;
	std::vector<int32> MappedMouseButtons;
	std::vector<std::function<void()>> Callbacks[(int32)InputEvent::MAX];
};

class InputManager
{
	DECLARE_SINGLE(InputManager)
	DECLARE_NONINSTANTIABLE(InputManager)

public:
	bool Init();
	void Update();
	void ClearListeners();

	// 인풋 매핑
	void MapAction(const std::string& actionName, int32 key);
	void MapMouseAction(const std::string& actionName, int32 button);

	// 폴링 방식의 인풋 처리
	bool GetButton(const std::string& actionName);
	bool GetButtonUp(const std::string& actionName);
	bool GetButtonDown(const std::string& actionName);
	const glm::vec2& GetMousePos() const { return m_mousePos; }

	//// 이벤트 등록 방식의 인풋 처리
	//template<typename T>
	//void BindAction(const std::string& actionName, T* instance, void(T::* func)());

private:
	static void DispatchKey(GLFWwindow* window, 
		int32 key, int32 scancode, int32 action, int32 mods);
	static void DispatchMouse(GLFWwindow* window, 
		int32 button, int32 action, int32 mods);
	static void DispatchCursorPos(GLFWwindow* window, 
		double xpos, double ypos);

private:
	void OnKey(int32 key, int32 scancode, int32 action, int32 mods);
	void OnMouse(int32 button, int32 action, int32 mods);
	void OnCursorPos(double xpos, double ypos);
	InputAction* GetOrCreateAction(const std::string& name);

	std::unordered_map<std::string, InputAction> m_actions;
	std::unordered_map<std::string, bool> m_prevActionStates;
	InputAction* m_keyMap[GLFW_KEY_LAST + 1]			= { nullptr };
	InputAction* m_mouseMap[GLFW_MOUSE_BUTTON_LAST + 1] = { nullptr };
	glm::vec2 m_mousePos							      { 0.0f, 0.0f };

/*========================//
//   ingui debug metohds  //
//========================*/
// TODO : 이거 스택 오버플로우 일어나는데 관련 문제 해결 필요
#pragma region FOR_IMGUI_DEBUG
private:
	static bool YieldKeyboardInputToImGUI();
	static bool YieldMouseInputToImGUI();
#pragma endregion
};

// #include "Managers/InputManager.inl"