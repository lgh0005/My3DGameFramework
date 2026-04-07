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
        RegisterDevice<MGFKeyboardDevice>();
        RegisterDevice<MGFMouseDevice>();

        // 2. 등록된 장치들 초기화
        uint32 activeDeviceCount = 0;
        for (auto& device : m_devices)
        {
            // Vector는 중간에 빈 슬롯이 있을 수 있으므로 nullptr 체크가 필수입니다.
            if (device)
            {
                if (device->Init() == false)
                {
                    MGF_LOG_ERROR("Failed to initialize input device.");
                    return false;
                }
                activeDeviceCount++;
            }
        }

        // 3. 인풋 인터페이스 콜백 연결
        MGFInputInterface::Install(window);

		// 4. 초기화 완료 로그
        MGF_LOG_INFO("InputManager initialized with {} devices.", activeDeviceCount);
        return true;
    }

    void InputManager::Update(GLFWwindow* window)
    {
        if (window == nullptr) return;
        for (auto& device : m_devices)
        {
            if (device)
                device->Update(window);
        }
    }

    void InputManager::Shutdown()
    {
        for (auto& device : m_devices)
        {
            if (device)
                device->Shutdown();
        }
        m_devices.clear();
    }
}