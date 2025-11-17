#include "WindowManager.h"
#include "EnginePch.h"
#include "Graphics/Context.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"
#include "Components/Camera.h"

void WindowManager::Init(GLFWwindow* handle)
{
	m_handle = handle;
}

void WindowManager::HandleFramebufferSizeChange(Context& context, int32 width, int32 height)
{    
    // 높이나 너비가 0이라면 프레임 버퍼의 크기 변환을 건너 뛴다.
    // TODO : 이후에 m_isIconified로 체크를 해도 되는 지 glfw의 콜백 함수 호출 순서를
    // 고려해본 다음에 수정 여부를 검토해야 한다.
    if (width == 0.0f || height == 0.0f) return;

    // TODO : 이후에 context에서 카메라를 분리할 수 있는 수단을 마련해야 함
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    /*Camera& camera = context.GetCamera();
    camera.SetProjection
    (
        45.0f,
        (float)width / (float)height,
        0.01f, 100.0f
    );*/

    Scene* activeScene = SCENE.GetActiveScene();
    if (activeScene)
    {
        Camera* camera = activeScene->GetMainCamera();

        // 2. [핵심] 카메라가 nullptr이 아닌지 "반드시" 확인합니다.
        if (camera)
        {
            // 3. 카메라가 존재할 때만 프로젝션을 업데이트합니다.
            camera->SetProjection(
                45.0f,
                (float)width / (float)height,
                0.01f, 100.0f
            );
        }
    }

    glViewport(0, 0, width, height);

    // 프레임 버퍼 세팅
    //FramebufferUPtr frameBuffer = Framebuffer::Create
    //(Texture::Create(width, height, GL_RGBA));
    // context.SetFrameBuffer(std::move(frameBuffer));
}

void WindowManager::HandleWindowIconified(int32 iconified)
{
    if (iconified == GLFW_TRUE)
    {
        SPDLOG_INFO("Window Minimized");
        m_isIconified = true;
    }
    else
    {
        SPDLOG_INFO("Window Restored");
        m_isIconified = false;
    }
}

