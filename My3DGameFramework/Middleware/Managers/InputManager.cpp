#include "CorePch.h"
#include "InputManager.h"

namespace MGF3D
{
    InputManager::InputManager() = default;
    InputManager::~InputManager() = default;

    /*==================================//
    //   default InputManager methods   //
    //==================================*/
    bool InputManager::Init(GLFWwindow* window)
    {
        if (!window)
        {
            MGF_LOG_FATAL("InputManager: Provided GLFW window handle is null.");
            return false;
        }

        glfwSetKeyCallback(window, DispatchKey);
        glfwSetMouseButtonCallback(window, DispatchMouse);
        glfwSetCursorPosCallback(window, DispatchCursorPos);

        MGF_LOG_INFO("InputManager: Initialized successfully.");
        return true;
    }

    void InputManager::Update(GLFWwindow* window)
    {
        if (!window) return;

        // 1. 고속 상태 스왑
        m_prevStates.swap(m_currentStates);

        // 2. 상태 갱신
        for (usize i = 0; i < m_actionList.Count(); ++i)
            m_currentStates[i] = CheckActionState(m_actionList[i].get(), window);
    }

    void InputManager::Shutdown()
    {
        MGF_LOG_INFO("InputManager: Shutting down and releasing resources.");

        m_actionIndexMap.Release();
        m_actionList.Release();
        m_currentStates.Release();
        m_prevStates.Release();

        for (auto& vec : m_keyMap) vec.Release();
        for (auto& vec : m_mouseMap) vec.Release();
    }

    /*===============================//
    //   mapping & binding actions   //
    //===============================*/
    void InputManager::MapAction(strview actionName, int32 key)
    {
        if (!CommonUtils::IsBetween(key, 0, GLFW_KEY_LAST)) return;

        Ptr<MGFInputAction> action = FetchAction(actionName);
        if (!action) return;

        action->AddKeyBinding(key);
        if (!m_keyMap[key].Contains(action))
            m_keyMap[key].PushBack(action);
    }

    void InputManager::MapMouseAction(strview actionName, int32 button)
    {
        if (!CommonUtils::IsBetween(button, 0, GLFW_MOUSE_BUTTON_LAST)) return;

        Ptr<MGFInputAction> action = FetchAction(actionName);
        if (!action) return;

        action->AddMouseButtonBinding(button);
        if (!m_mouseMap[button].Contains(action))
            m_mouseMap[button].PushBack(action);
    }

    void InputManager::BindAction(strview actionName, InputEvent event, const MGFInputAction::InputCallback& callback)
    {
        Ptr<MGFInputAction> action = FetchAction(actionName);
        if (!action) return;
        action->BindCallback(event, callback);
    }

    /*==========================//
    //   button state getters   //
    //==========================*/
    bool InputManager::GetButton(strview actionName)
    {
        StringHash hash(actionName);
        int32* pIndex = m_actionIndexMap.Find(hash);
        if (pIndex) return m_currentStates[*pIndex];
        return false;
    }

    bool InputManager::GetButtonDown(strview actionName)
    {
        StringHash hash(actionName);
        int32* pIndex = m_actionIndexMap.Find(hash);
        if (pIndex) return m_currentStates[*pIndex] && !m_prevStates[*pIndex];
        return false;
    }

    bool InputManager::GetButtonUp(strview actionName)
    {
        StringHash hash(actionName);
        int32* pIndex = m_actionIndexMap.Find(hash);
        if (pIndex) return !m_currentStates[*pIndex] && m_prevStates[*pIndex];
        return false;
    }

    /*===========================//
    //   Input event callbacks   //
    //===========================*/
    void InputManager::DispatchKey(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
    {
        MGF_INPUT.OnKey(key, scancode, action, mods);
    }

    void InputManager::DispatchMouse(GLFWwindow* window, int32 button, int32 action, int32 mods)
    {
        MGF_INPUT.OnMouse(button, action, mods);
    }

    void InputManager::DispatchCursorPos(GLFWwindow* window, double xpos, double ypos)
    {
        MGF_INPUT.OnCursorPos(xpos, ypos);
    }

    void InputManager::OnKey(int32 key, int32 scancode, int32 action, int32 mods)
    {
        if (key < 0 || key > GLFW_KEY_LAST) return;

        const auto& actions = m_keyMap[key];
        for (const auto& inputAction : actions)
        {
            switch (action)
            {
                case GLFW_PRESS:
                {
                    inputAction->Invoke(InputEvent::Pressed);
                    break;
                }

                case GLFW_RELEASE:
                {
                    inputAction->Invoke(InputEvent::Released);
                    break;
                }
            }
        }
    }

    void InputManager::OnMouse(int32 button, int32 action, int32 mods)
    {
        if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return;

        const auto& actions = m_mouseMap[button];
        for (const auto& inputAction : actions)
        {
            switch (action)
            {
                case GLFW_PRESS:
                {
                    inputAction->Invoke(InputEvent::Pressed);
                    break;
                }

                case GLFW_RELEASE:
                {
                    inputAction->Invoke(InputEvent::Released);
                    break;
                }
            }
        }
    }

    void InputManager::OnCursorPos(double xpos, double ypos)
    {
        m_mousePos = vec2(static_cast<float>(xpos), static_cast<float>(ypos));
    }

    Ptr<MGFInputAction> InputManager::FetchAction(strview name)
    {
        StringHash hash(name);
        int32* pIndex = m_actionIndexMap.Find(hash);

        // 1. 이미 있으면 기존 것 반환
        if (pIndex) return m_actionList[*pIndex].get();

        // 2. 없으면 새로 생성 및 배열 확장
        MGFInputActionUPtr newAction = MGFInputActionUPtr(new MGFInputAction(SString(name)));
        Ptr<MGFInputAction> actionPtr = newAction.get();
        int32 newIndex = static_cast<int32>(m_actionList.Count());

        m_actionList.PushBack(std::move(newAction));
        m_currentStates.PushBack(false);
        m_prevStates.PushBack(false);
        m_actionIndexMap.Insert(hash, newIndex);

        return actionPtr;
    }

    /*==============================//
    //   action setters & getters   //
    //==============================*/
    Ptr<MGFInputAction> InputManager::GetAction(strview name)
    {
        StringHash hash(name);
        const int32* pIndex = m_actionIndexMap.Find(hash);
        
        if (pIndex) return m_actionList[*pIndex].get();
        return nullptr;
    }

    bool InputManager::CheckActionState(const Ptr<MGFInputAction>& action, GLFWwindow* window)
    {
        if (!action || !window) return false;

        // 1. 해당 액션에 매핑된 키보드 키 중 하나라도 눌려있으면 true
        for (int32 key : action->GetMappedKeys())
        {
            if (glfwGetKey(window, key) == GLFW_PRESS) 
                return true;
        }

        // 2. 해당 액션에 매핑된 마우스 버튼 중 하나라도 눌려있으면 true
        for (int32 btn : action->GetMappedMouseButtons())
        {
            if (glfwGetMouseButton(window, btn) == GLFW_PRESS) 
                return true;
        }

        return false;
    }

}