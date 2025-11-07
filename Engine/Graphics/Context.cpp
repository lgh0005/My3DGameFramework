#include "EnginePch.h"
#include "Context.h"
#include "Graphics/Shader.h"
#include "Graphics/Program.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Image.h"
#include "Graphics/Texture.h"
#include "Components//Transform.h"
#include "Components/Camera.h"

Context::~Context() = default;

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init()) return nullptr;
    return std::move(context);
}

void Context::Render()
{
    IMGUI.BeginFrame();

    // imgui context
    if (ImGui::Begin("My First ImGUI Window"))
    {
        ImGui::Text("Hello, ImGui!");
    }ImGui::End();

    // render context
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cubeTransform1->SetRotation(glm::vec3(1.0f, 2.0f, 0.0f),
        glm::radians((float)glfwGetTime() * 120.0f));

    m_cubeTransform2->SetRotation(glm::vec3(2.0f, 4.0f, 0.0f),
        glm::radians((float)glfwGetTime() * 120.0f));

    auto projection = m_camera->GetProjectionMatrix();
    auto view = m_camera->GetViewMatrix();

    // 큐브 #1
    auto cubeModel1 = m_cubeTransform1->GetModelMatrix();
    auto transform1 = projection * view * cubeModel1;
    m_program->SetUniform("transform", transform1);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 큐브 #2
    auto cubeModel2 = m_cubeTransform2->GetModelMatrix();
    auto transform2 = projection * view * cubeModel2;
    m_program->SetUniform("transform", transform2);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
   
    IMGUI.EndFrame();
}

bool Context::Init()
{
    // 1. 큐브 생성
    {
        float vertices[] = {
          -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
           0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
           0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
          -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

          -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
           0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
           0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
          -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

          -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
          -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
          -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
          -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

           0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
           0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
           0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
           0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

          -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
           0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
           0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
          -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

          -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
           0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
           0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
          -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
        };

        uint32 indices[] = {
           0,  2,  1,  2,  0,  3,
           4,  5,  6,  6,  7,  4,
           8,  9, 10, 10, 11,  8,
          12, 14, 13, 14, 12, 15,
          16, 17, 18, 18, 19, 16,
          20, 22, 21, 22, 20, 23,
        };

        m_vertexLayout = VertexLayout::Create();
        m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 120);
        m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
        m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 3 * sizeof(float));
        m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32) * 36);
    }

    // 2. 렌더링 파이프라인 생성
    {
        ShaderPtr vertShader = Shader::CreateFromFile("./Resources/Shaders/texture.vert", GL_VERTEX_SHADER);
        ShaderPtr fragShader = Shader::CreateFromFile("./Resources/Shaders/texture.frag", GL_FRAGMENT_SHADER);
        if (!vertShader || !fragShader) return false;
        SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
        SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

        m_program = Program::Create({ fragShader, vertShader });
        if (!m_program) return false;
        SPDLOG_INFO("program id: {}", m_program->Get());

        glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    }

    // 이미지 로드
    auto image1 = Image::Load("./Resources/Images/container.jpg");
    if (!image1)  return false;
    m_texture1 = Texture::CreateFromImage(image1.get());

    auto image2 = Image::Load("./Resources/Images/awesomeface.png");
    if (!image2)  return false;
    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    m_cubeTransform1 = Transform::Create();

    m_cubeTransform2 = Transform::Create();
    m_cubeTransform2->SetPosition(glm::vec3(2.0f, 2.0f, 0.0f));

    m_camera = Camera::Create();
    m_camera->GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    m_camera->SetProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
        0.01f, 100.0f);

    return true;
}

// TEMP : 이후에는 스크립트로 이를 조정해야 한다.
/*=====================//
//  camera controller  //
//=====================*/
void Context::ProcessInput(GLFWwindow* window)
{
    if (!m_cameraControl) return;

    // 1. m_camera에서 Transform 참조를 가져옵니다.
    auto& camTransform = m_camera->GetTransform();

    // 2. Transform에서 직접 방향 벡터를 가져옵니다.
    glm::vec3 forward = camTransform.GetForwardVector();
    glm::vec3 right = camTransform.GetRightVector();
    glm::vec3 up = camTransform.GetUpVector();

    const float cameraSpeed = 5.0f * TIME.GetDeltaTime();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camTransform.Translate(cameraSpeed * forward);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camTransform.Translate(-cameraSpeed * forward);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camTransform.Translate(cameraSpeed * right);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camTransform.Translate(-cameraSpeed * right);

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camTransform.Translate(cameraSpeed * up);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camTransform.Translate(-cameraSpeed * up);
}

void Context::MouseMove(double x, double y)
{
    if (!m_cameraControl) return;

    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.1f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_camera->GetTransform().SetRotation(glm::vec3(m_cameraPitch, m_cameraYaw, 0.0f));

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT) 
    {
        SPDLOG_INFO("Pressed Right mouse button.");
        if (action == GLFW_PRESS) 
        {
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) 
        {
            m_cameraControl = false;
        }
    }
}
