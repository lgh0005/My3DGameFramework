#include "WindowManager.h"
#include "EnginePch.h"
#include "Graphics/Context.h"
#include "Components/Camera.h"

void WindowManager::Init(GLFWwindow* handle)
{
	m_handle = handle;
}

void WindowManager::HandleFramebufferSizeChange(Context& context, int32 width, int32 height)
{
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    Camera& camera = context.GetCamera();
    camera.SetProjection
    (
        45.0f,
        (float)width / (float)height,
        0.01f, 100.0f
    );
    glViewport(0, 0, width, height);
}

