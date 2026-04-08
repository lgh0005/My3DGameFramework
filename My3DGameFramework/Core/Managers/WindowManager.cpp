#include "CorePch.h"
#include "WindowManager.h"
#include "Window/MGFWindow.h"
#include <glfw/glfw3.h>

namespace MGF3D
{
    WindowManager::WindowManager() = default;
    WindowManager::~WindowManager() = default;

    bool WindowManager::Init(int32 width, int32 height, const String& title, int32 vsync)
    {
        // 0. GLFW 전역 에러 콜백 등록
        glfwSetErrorCallback(WindowManager::ErrorCallback);

        // 1. GLFW 라이브러리 전역 초기화
        if (glfwInit() == GLFW_FALSE)
        {
            cstr description = nullptr;
            glfwGetError(&description);
            MGF_LOG_FATAL("WindowManager: Failed to initialize GLFW - {}", description);
            return false;
        }

        // 2. 메인 윈도우 인스턴스 생성
        m_mainWindow = MGFWindow::Create(width, height, title, vsync);
        if (m_mainWindow == nullptr)
        {
            MGF_LOG_FATAL("WindowManager: Failed to create main window.");
            glfwTerminate();
            return false;
        }

        return true;
    }

    void WindowManager::Update()
    {
        glfwPollEvents();
    }

    void WindowManager::Shutdown()
    {
        if (m_mainWindow != nullptr)
        {
            m_mainWindow->Destroy();
            m_mainWindow.reset();
        }
        glfwTerminate();
        MGF_LOG_INFO("WindowManager: GLFW terminated and window destroyed.");
    }

    bool WindowManager::ShouldClose() const
    {
        if (m_mainWindow != nullptr) 
            return glfwWindowShouldClose(m_mainWindow->GetHandle());
        return true;
    }

    GLFWwindow* WindowManager::GetNativeHandle() const
    {
        if (m_mainWindow != nullptr)
            return m_mainWindow->GetHandle();
        return nullptr;
    }

    int32 WindowManager::GetWindowWidth() const
    {
        if (m_mainWindow != nullptr) return m_mainWindow->GetWidth();
        return 0;
    }

    int32 WindowManager::GetWindowHeight() const
    {
        if (m_mainWindow != nullptr) return m_mainWindow->GetHeight();
        return 0;
    }

    void WindowManager::SwapWindowBuffers()
    {
        m_mainWindow->SwapBuffers();
    }

    void WindowManager::ErrorCallback(int32 error, cstr description)
    {
        MGF_LOG_ERROR("WindowManager [GLFW Error {0}]: {1}", error, description);
    }
}