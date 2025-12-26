#include "WindowManager.h"
#include "EnginePch.h"
#include "Resources/Texture.h"
#include "Components/Camera.h"

bool WindowManager::Init(int32 width, int32 height, const std::string& title)
{
    // glfw 라이브러리 초기화
    if (!glfwInit())
    {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return false;
    }

    // OpenGL 버전 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 윈도우 생성
    SPDLOG_INFO("Create glfw window");
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);
    glfwSwapInterval(0); // TEMP : v-sync 잠시 끄기

    // glad를 활용한 OpenGL 함수 로딩
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return false;
    }
    auto glVersion = glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", reinterpret_cast<const char*>(glVersion));

    // glfw 콜백 함수 등록
    RegisterStaticEventCallbacks();

    // SPIR-V 확장 지원 체크
    if (glfwExtensionSupported("GL_ARB_gl_spirv")) SPDLOG_INFO("SPIR-V supported!");
    else SPDLOG_WARN("SPIR-V not supported on this system!");

    return true;
}

WindowManager* WindowManager::GetWindowUserPtr()
{
    return static_cast<WindowManager*>(glfwGetWindowUserPointer(m_window));
}

void WindowManager::RegisterStaticEventCallbacks()
{
    glfwSetFramebufferSizeCallback(m_window, HandleFramebufferSizeChange);
    glfwSetWindowIconifyCallback(m_window, HandleWindowIconified);
}

void WindowManager::DestroyWindow()
{
    // glfw 정리
    glfwTerminate();
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

/*=====================//
//   window callbacks  //
//=====================*/
void WindowManager::HandleFramebufferSizeChange(GLFWwindow* window, int32 width, int32 height)
{    
    // 높이나 너비가 0이라면 프레임 버퍼의 크기 변환을 건너 뛴다.
    if (width <= 0 || height <= 0) return;

    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    
    /// <summary>
    /// 1. 렌더러 (FBO 재생성, Viewport 설정)
    /// WindowManager -> Renderer -> RenderPipeline을 타고 가서 Viewport를 변경
    /// </summary>
    RENDER.OnResize(width, height);
   
    /// <summary>
    /// 2. 화면 크기에 맞게 종횡비를 맞추면서 렌더링
    /// Scene의 카메라 컴포넌트의 종횡비를 변경
    /// </summary>
    SCENE.OnScreenResize(width, height);
}

void WindowManager::HandleWindowIconified(GLFWwindow* window, int32 iconified)
{
    if (iconified == GLFW_TRUE)
    {
        SPDLOG_INFO("Window Minimized");
        WINDOW.SetIconified(true);
    }
    else
    {
        SPDLOG_INFO("Window Restored");
        WINDOW.SetIconified(false);
    }
}