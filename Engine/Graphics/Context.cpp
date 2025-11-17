#include "EnginePch.h"
#include "Context.h"

#include "Graphics/Shader.h"
#include "Graphics/Program.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Mesh.h"
#include "Graphics/Image.h"
#include "Graphics/Texture.h"
#include "Graphics/Model.h"
#include "Graphics/Animation.h"
#include "Graphics/Material.h"
#include "Graphics/FrameBuffer.h"

#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/PointLight.h"
#include "Components/DirectionalLight.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"

Context::~Context() = default;

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init()) return nullptr;
    return std::move(context);
}

void Context::Update()
{
    if (Scene* activeScene = SCENE.GetActiveScene())
    {
        activeScene->Update(); // (DevScene::Update() 호출)
    }
}

void Context::Render()
{
    Scene* activeScene = SCENE.GetActiveScene();
    Renderer* activeRenderer = SCENE.GetActiveRenderer();
    if (activeScene && activeRenderer)
    {
        activeRenderer->Render(activeScene);
    }

    // 2. ImGui 렌더링 (Context가 담당)
    {
        IMGUI.BeginFrame();

        {
            // imgui context #1 : 카메라
            if (IMGUI.Begin("Camera Parameters"))
            {
                if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
                    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

                ImGui::Separator();

                // [수정] m_camera가 유효한지 확인하고 GetOwner() 사용
                if (m_camera)
                {
                    auto& camTransform = m_camera->GetTransform();
                    glm::vec3 cameraPos = camTransform.GetPosition();
                    bool posChanged = ImGui::DragFloat3("camera pos", glm::value_ptr(cameraPos), 0.01f);
                    bool rotChanged = ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
                    rotChanged |= ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);

                    if (posChanged) camTransform.SetPosition(cameraPos);
                    if (rotChanged) camTransform.SetRotation(glm::vec3(m_cameraPitch, m_cameraYaw, 0.0f));

                    ImGui::Separator();
                    if (ImGui::Button("reset camera"))
                    {
                        m_cameraYaw = 0.0f;
                        m_cameraPitch = 0.0f;
                        camTransform.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
                        camTransform.SetRotation(glm::vec3(m_cameraPitch, m_cameraYaw, 0.0f));
                    }
                }
            } IMGUI.End();

            // imgui context #2 : 조명
            if (IMGUI.Begin("Light Parameters"))
            {
                // [수정] m_spotLight가 유효한지 확인하고 GetOwner() 사용
                if (m_spotLight)
                {
                    auto& lightTransform = m_spotLight->GetTransform();
                    glm::vec3 lightPos = lightTransform.GetPosition();
                    if (ImGui::DragFloat3("position", glm::value_ptr(lightPos), 0.01f))
                        lightTransform.SetPosition(lightPos);

                    // [참고] SpotLight의 방향(Direction)은 이제 Transform의 회전값으로 제어해야 합니다.
                    // (m_spotLight->GetDirection() / SetDirection()은 제거됨)
                    // (예: ImGui::DragFloat3("rotation", ...)으로 lightTransform.SetRotation(...) 호출)
                    glm::vec3 currentRotation = lightTransform.GetRotationEuler();
                    bool rotationChanged = ImGui::DragFloat3("rotation", glm::value_ptr(currentRotation), 0.5f);
                    if (rotationChanged) lightTransform.SetRotation(currentRotation);

                    glm::vec2 lightCutoff = m_spotLight->GetCutoff();
                    if (ImGui::DragFloat("cutoff", glm::value_ptr(lightCutoff), 0.1))
                        m_spotLight->SetCutoff(lightCutoff);

                    float lightDist = m_spotLight->GetDistance();
                    if (ImGui::DragFloat("distance", &lightDist, 0.01))
                        m_spotLight->SetDistance(lightDist);

                    glm::vec3 ambient = m_spotLight->GetAmbient();
                    if (ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient)))
                        m_spotLight->SetAmbient(ambient);

                    glm::vec3 diffuse = m_spotLight->GetDiffuse();
                    if (ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse)))
                        m_spotLight->SetDiffuse(diffuse);

                    glm::vec3 specular = m_spotLight->GetSpecular();
                    if (ImGui::ColorEdit3("Specular", glm::value_ptr(specular)))
                        m_spotLight->SetSpecular(specular);
                }
            } IMGUI.End();
        }

        // imgui context #3 : 감마
        if (IMGUI.Begin("Gamma correction"))
        {
            // [수정] 활성 렌더러의 GetGamma() 호출
            if (activeRenderer)
            {
                ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
            }
        } IMGUI.End();

        IMGUI.EndFrame();
    }
}

bool Context::Init()
{
    SCENE.LoadScene("DevScene");

    // [수정] 씬 로드 및 ImGui 포인터 캐시
    Scene* activeScene = SCENE.GetActiveScene();
    if (activeScene)
    {
        m_camera = activeScene->GetMainCamera();

        // 씬에서 스포트라이트를 찾아 m_spotLight에 캐시
        for (auto* light : activeScene->GetLights()) 
        {
            if (light->GetLightType() == LightType::Spot) 
            {
                m_spotLight = static_cast<SpotLight*>(light);
                break;
            }
        }
    }

    return true;
}

// TEMP : 이후에는 스크립트로 이를 조정해야 한다.
/*=====================//
//  camera controller  //
//=====================*/
#pragma region Camera Controller

void Context::ProcessInput(GLFWwindow* window)
{
    if (!m_cameraControl) return;

    // 1. m_camera에서 Transform 참조를 가져옵니다.
    Scene* scene = SCENE.GetActiveScene(); if (!scene) return;
    Camera* camera = scene->GetMainCamera(); if (!camera) return;
    auto& camTransform = camera->GetTransform();

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

    Scene* scene = SCENE.GetActiveScene(); if (!scene) return;
    Camera* camera = scene->GetMainCamera(); if (!camera) return;

    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.1f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    camera->GetTransform().SetRotation(glm::vec3(m_cameraPitch, m_cameraYaw, 0.0f));

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

#pragma endregion

