#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum class EInputEvent
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
	std::vector<std::function<void()>> Callbacks[(int32)EInputEvent::MAX];
};

class InputManager
{
	DECLARE_SINGLE(InputManager)

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

	// 이벤트 등록 방식의 인풋 처리
	template<typename T>
	void BindAction(const std::string& actionName, T* instance, void(T::* func)());

private:
	static void DispatchKey(GLFWwindow* window, 
		int32 key, int32 scancode, int32 action, int32 mods);
	static void DispatchMouse(GLFWwindow* window, 
		int32 button, int32 action, int32 mods);

private:
	void OnKey(int32 key, int32 scancode, int32 action, int32 mods);
	void OnMouse(int32 button, int32 action, int32 mods);
	InputAction* GetOrCreateAction(const std::string& name);

	std::unordered_map<std::string, InputAction> m_actions;
	std::unordered_map<std::string, bool> m_prevActionStates;
	InputAction* m_keyMap[GLFW_KEY_LAST + 1]			= { nullptr };
	InputAction* m_mouseMap[GLFW_MOUSE_BUTTON_LAST + 1] = { nullptr };

// TEMP : ImGUI로 디버그를 할 때 쓰이는 메서드.
#pragma region FOR_IMGUI_DEBUG
private:
	static bool YieldKeyboardInputToImGUI(GLFWwindow* window, 
		int32 key, int32 scancode, int32 action, int32 mods);
	static bool YieldMouseInputToImGUI(GLFWwindow* window,
		int32 button, int32 action, int32 mods);
#pragma endregion
};

/*===================================//
//   input manager template inlines  //
//===================================*/
#pragma region INPUT_MANAGER_INLINES
template<typename T>
inline void InputManager::BindAction(const std::string& actionName, T* instance, void(T::* func)())
{
	auto boundFunc = [instance, func]() { (instance->*func)(); };
	GetOrCreateAction(actionName)->Callbacks[(int32)eventType].push_back(boundFunc);
}
#pragma endregion