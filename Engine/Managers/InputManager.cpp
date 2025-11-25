#include "InputManager.h"
#include "EnginePch.h"

// TODO : ImGUI 인풋을 먹을 때는 게임에는 인풋 영향이 가지 않도록 
// 만들 필요도 있겠으나 ImGUI를 간단한 디버그 용도로 쓰고 실제 인게임에는
// 등장시키지 않을 것이라면 크게 신경쓰지 않아도 됨.
/*====================//
//    user methods    //
//====================*/
void InputManager::MapAction(const std::string& actionName, int32 key)
{
    InputAction* action = GetOrCreateAction(actionName);
    m_keyMap[key] = action;
    action->MappedKeys.push_back(key);
}

void InputManager::MapMouseAction(const std::string& actionName, int32 button)
{
    InputAction* action = GetOrCreateAction(actionName);
    m_mouseMap[button] = action;
    action->MappedMouseButtons.push_back(button);
}

bool InputManager::GetButton(const std::string& actionName)
{
    if (m_actions.find(actionName) == m_actions.end()) return false;

    const InputAction& action = m_actions[actionName];
    GLFWwindow* window = WINDOW.GetWindow();

    // 1. 키보드 체크
    for (int32 key : action.MappedKeys)
    {
        if (glfwGetKey(window, key) == GLFW_PRESS) return true;
    }

    // 2. 마우스 체크
    for (int32 btn : action.MappedMouseButtons)
    {
        if (glfwGetMouseButton(window, btn) == GLFW_PRESS) return true;
    }

    return false;
}

bool InputManager::GetButtonDown(const std::string& actionName)
{
    bool isPressed = GetButton(actionName);
    bool wasPressed = m_prevActionStates[actionName];
    return isPressed && !wasPressed;
}

bool InputManager::GetButtonUp(const std::string& actionName)
{
    bool isPressed = GetButton(actionName);
    bool wasPressed = m_prevActionStates[actionName];
    return !isPressed && wasPressed;
}

/*============================================//
//    default public input manager methods    //
//============================================*/
bool InputManager::Init()
{
    GLFWwindow* window = WINDOW.GetWindow();
    if (!window)
    {
        SPDLOG_ERROR("WindowManager does not have glfw window handle.\
        Check if InputManager initializes before WindowManager.");
        return false;
    }
	
    glfwSetKeyCallback(window, DispatchKey);
    glfwSetMouseButtonCallback(window, DispatchMouse);
    return true;
}

void InputManager::Update()
{
    for (auto& action : m_actions)
    {
        const std::string& name = action.first;
        m_prevActionStates[name] = GetButton(name);
    }
}

void InputManager::ClearListeners()
{
    for (auto& actionPair : m_actions)
    {
        for (int i = 0; i < (int32)EInputEvent::MAX; ++i)
            actionPair.second.Callbacks[i].clear();
    }
}

/*==========================//
//   input event callbacks  //
//==========================*/
InputAction* InputManager::GetOrCreateAction(const std::string& name)
{
    InputAction& action = m_actions[name]; // 없으면 생성
    if (action.Name.empty()) action.Name = name;
    return &action;
}

void InputManager::DispatchKey(GLFWwindow* window, 
    int32 key, int32 scancode, int32 action, int32 mods)
{
    if (YieldKeyboardInputToImGUI(window, key, scancode, action, mods))
        return;
    INPUT.OnKey(key, scancode, action, mods);
}

void InputManager::OnKey(int32 key, int32 scancode, int32 action, int32 mods)
{
    // 1. 키에 매핑된 액션 리스트 가져오기
    InputAction* actionPtr = m_keyMap[key];
    if (!actionPtr) return;
   
    //  Pressed/Released만 즉시 실행
    switch (action)
    {
        case GLFW_PRESS:
        {
            for (const auto& func : actionPtr->Callbacks[(int32)EInputEvent::Pressed])
                if (func) func();
            break;
        }

        case GLFW_RELEASE:
        {
            for (const auto& func : actionPtr->Callbacks[(int32)EInputEvent::Released])
                if (func) func();
            break;
        }
    }
}

void InputManager::DispatchMouse(GLFWwindow* window,
    int32 button, int32 action, int32 mods)
{
    if (YieldMouseInputToImGUI(window, button, action, mods))
        return;

    INPUT.OnMouse(button, action, mods);
}

void InputManager::OnMouse(int32 button, int32 action, int32 mods)
{
    InputAction* actionPtr = m_mouseMap[button];
    if (!actionPtr) return;

    switch (action)
    {
        case GLFW_PRESS:
        {
            for (const auto& func : actionPtr->Callbacks[(int32)EInputEvent::Pressed])
                if (func) func();
            break;
        }

        case GLFW_RELEASE:
        {
            for (const auto& func : actionPtr->Callbacks[(int32)EInputEvent::Released])
                if (func) func();
            break;
        }
    }
}

/*====================//
//   for imgui debug  //
//====================*/
#pragma region FOR_IMGUI_DEBUG
bool InputManager::YieldKeyboardInputToImGUI(GLFWwindow* window,
    int32 key, int32 scancode, int32 action, int32 mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool InputManager::YieldMouseInputToImGUI(GLFWwindow* window,
    int32 button, int32 action, int32 mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    return ImGui::GetIO().WantCaptureMouse;
}
#pragma endregion