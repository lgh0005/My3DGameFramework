#include "CorePch.h"
#include "WindowManager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MGF3D
{
    WindowManager::WindowManager() = default;
    WindowManager::~WindowManager() = default;

    bool WindowManager::Init(int32 width, int32 height, const LString& title)
    {
        MGF_LOG_INFO("WindowManager : Initializing main window.");

        // 1. MGFWindow의 정적 팩토리를 통해 윈도우 생성
        m_mainWindow = MGFWindow::Create(width, height, title);
        if (!m_mainWindow)
        {
            MGF_LOG_FATAL("WindowManager : Failed to create main window instance!");
            return false;
        }

        return true;
    }

    void WindowManager::Shutdown()
    {
        if (m_mainWindow)
        {
            m_mainWindow->Destroy();
            m_mainWindow.Reset();
            MGF_LOG_INFO("WindowManager : Main window shutdown.");
        }
    }

    void WindowManager::Update()
    {
        if (m_mainWindow && !m_mainWindow->IsIconified())
            m_mainWindow->SwapBuffers();
    }

    bool WindowManager::ShouldClose() const
    {
        if (!m_mainWindow) return true;
        return glfwWindowShouldClose(GetNativeWindow());
    }

    // TODO : 런타임 전용 CommonUtils::Select도 필요하다.
    Ptr<GLFWwindow> WindowManager::GetNativeWindow() const
    {
        return m_mainWindow ? m_mainWindow->GetHandle() : nullptr;
    }

    int32 WindowManager::GetWindowWidth() const
    {
        return m_mainWindow ? m_mainWindow->GetWidth() : 0;
    }

    int32 WindowManager::GetWindowHeight() const
    {
        return m_mainWindow ? m_mainWindow->GetHeight() : 0;
    }

    bool WindowManager::IsIconified() const
    {
        return m_mainWindow ? m_mainWindow->IsIconified() : false;
    }

    void WindowManager::SetResizeCallback(const MGFWindow::ResizeCallback& callback)
    {
        if (m_mainWindow) m_mainWindow->SetResizeCallback(callback);
    }

    void WindowManager::SetIconifyCallback(const MGFWindow::IconifyCallback& callback)
    {
        if (m_mainWindow) m_mainWindow->SetIconifyCallback(callback);
    }
}