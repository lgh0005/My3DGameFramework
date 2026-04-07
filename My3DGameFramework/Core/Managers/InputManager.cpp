#include "CorePch.h"
#include "InputManager.h"
#include "Input/MGFInputDevice.h"
#include "Input/Devices/MGFKeyboardDevice.h"
#include "Input/Devices/MGFMouseDevice.h"
#include "Input/MGFInputInterface.h"
#include <GLFW/glfw3.h>

namespace MGF3D
{
    InputManager::InputManager() = default;
    InputManager::~InputManager() = default;

    bool InputManager::Init(GLFWwindow* window)
    {
        if (window == nullptr) return false;

        // 1. 기본 장치 등록 및 캐싱
        RegisterDevice<MGFKeyboardDevice>("Keyboard");
        RegisterDevice<MGFMouseDevice>("Mouse");

        // 2. GLFW 인풋 콜백 연결
        for (auto& [hash, device] : m_devices)
        {
            if (device->Init() == false)
            {
                MGF_LOG_ERROR("Failed to initialize input device.");
                return false;
            }
        }

        // 3. 인풋 인터페이스 콜백 연결
        MGFInputInterface::Install(window);

		// 4. 초기화 완료 로그
        MGF_LOG_INFO("InputManager initialized with {} devices.", m_devices.size());
        return true;
    }

    void InputManager::Update(GLFWwindow* window)
    {
        if (window == nullptr) return;
        for (auto& [hash, device] : m_devices)
            device->Update(window);
    }

    void InputManager::Shutdown()
    {
        for (auto& [hash, device] : m_devices)
            device->Shutdown();
        m_devices.clear();
    }
}