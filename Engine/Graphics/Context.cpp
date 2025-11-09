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

void Context::Render()
{
    IMGUI.BeginFrame();
    
    {
        // imgui 컨텍스트들
        {
            // imgui context #1 : 카메라에 대한 imgui 창.
            if (IMGUI.Begin("Camera Parameters")) {
                if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
                    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

                ImGui::Separator();

                glm::vec3 cameraPos = m_camera->GetTransform().GetPosition();
                bool posChanged = ImGui::DragFloat3("camera pos", glm::value_ptr(cameraPos), 0.01f);
                bool rotChanged = ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
                rotChanged |= ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);

                if (posChanged) m_camera->GetTransform().SetPosition(cameraPos);
                if (rotChanged) m_camera->GetTransform().SetRotation(glm::vec3(m_cameraPitch, m_cameraYaw, 0.0f));

                ImGui::Separator();
                if (ImGui::Button("reset camera"))
                {
                    m_cameraYaw = 0.0f;
                    m_cameraPitch = 0.0f;
                    m_camera->GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
                    m_camera->GetTransform().SetRotation(glm::vec3(m_cameraPitch, m_cameraYaw, 0.0f));
                }
            } IMGUI.End();

            // imgui context #2 : 광원에 대한 imgui 창.
            if (IMGUI.Begin("Light Parameters"))
            {
                glm::vec3 lightPos = m_spotLight->GetTransform().GetPosition();
                if (ImGui::DragFloat3("position", glm::value_ptr(lightPos), 0.01f))
                    m_spotLight->GetTransform().SetPosition(lightPos);

                glm::vec3 lightDir = m_spotLight->GetDirection();
                if (ImGui::DragFloat3("direction", glm::value_ptr(lightDir), 0.01))
                    m_spotLight->SetDirection(lightDir);

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

            } IMGUI.End();

            // imgui context #3 : 머티리얼에 대한 imgui 창 #2.
            if (IMGUI.Begin("Material Parameters (Textures)"))
            {
                ImGui::DragFloat("Shininess", &m_material.shininess, 1.0f, 1.0f, 256.0f);
            } IMGUI.End();
        }

        // render context
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // 카메라 시점 행렬
        auto cameraPos = m_camera->GetTransform().GetPosition();
        auto projection = m_camera->GetProjectionMatrix();
        auto view = m_camera->GetViewMatrix();

        // 조명 큐브와 조명 유니폼 변수를 설정
        {
            m_lighting2->Use();
            m_lighting2->SetUniform("viewPos", cameraPos);

            m_lighting2->SetUniform("light.position", m_spotLight->GetTransform().GetPosition());
            m_lighting2->SetUniform("light.direction", m_spotLight->GetDirection());
            auto cutoff = m_spotLight->GetCutoff();
            m_lighting2->SetUniform("light.cutoff", glm::vec2
            (
                cosf(glm::radians(cutoff[0])),
                cosf(glm::radians(cutoff[0] + cutoff[1]))));

            m_lighting2->SetUniform("light.attenuation", Utils::GetAttenuationCoeff(m_spotLight->GetDistance()));
            m_lighting2->SetUniform("light.ambient", m_spotLight->GetAmbient());
            m_lighting2->SetUniform("light.diffuse", m_spotLight->GetDiffuse());
            m_lighting2->SetUniform("light.specular", m_spotLight->GetSpecular());

            m_lighting2->SetUniform("material.diffuse", 0);
            m_lighting2->SetUniform("material.specular", 1);
            glActiveTexture(GL_TEXTURE0);
            m_material.diffuse->Bind();
            glActiveTexture(GL_TEXTURE1);
            m_material.specular->Bind();
            m_lighting2->SetUniform("material.shininess", m_material.shininess);
        }

        // 큐브 물체
        {
            // 큐브 #1
            m_cubeTransform1->SetRotation(glm::vec3(1.0f, 2.0f, 0.0f),
                glm::radians((float)glfwGetTime() * 30.0f));
            auto cubeModel1 = m_cubeTransform1->GetModelMatrix();
            auto transform1 = projection * view * cubeModel1;
            m_lighting2->SetUniform("transform", transform1);
            m_lighting2->SetUniform("modelTransform", cubeModel1);
            m_box->Draw(m_lighting2.get());

            // 큐브 #2
            m_cubeTransform2->SetRotation(glm::vec3(2.0f, 4.0f, 0.0f),
                glm::radians((float)glfwGetTime() * 30.0f));
            auto cubeModel2 = m_cubeTransform2->GetModelMatrix();
            auto transform2 = projection * view * cubeModel2;
            m_lighting2->SetUniform("transform", transform2);
            m_lighting2->SetUniform("modelTransform", cubeModel2);
            m_box->Draw(m_lighting2.get());

            // 조명 위치를 표시하는 큐브 #1
            {
                m_spotLight->GetTransform().SetScale(glm::vec3(0.2f));
                auto lightModel1 = m_spotLight->GetTransform().GetModelMatrix();
                auto lightTransform1 = projection * view * lightModel1;

                m_simpleProgram->SetUniform("transform", lightTransform1);

                // 반사, 확산 모두 끄기
                m_simpleProgram->Use();
                m_simpleProgram->SetUniform("color", glm::vec4(m_spotLight->GetAmbient() + m_spotLight->GetDiffuse(), 1.0f));
                m_simpleProgram->SetUniform("transform", lightTransform1);

                m_box->Draw(m_simpleProgram.get());
            }

            // 모델 #1
            {
                m_skinningProgram->Use();

                // 1. 셰이더에 MVP 행렬 설정
                auto modelTransform = m_modelTransform->GetModelMatrix();
                m_skinningProgram->SetUniform("projection", projection);
                m_skinningProgram->SetUniform("view", view);
                m_skinningProgram->SetUniform("model", modelTransform);

                // 2. 셰이더에 뼈(Bone) 행렬 배열 설정
                auto finalMatrices = m_animator->GetFinalBoneMatrices();
                for (int i = 0; i < finalMatrices.size(); ++i)
                {
                    // "finalBoneMatrices[0]", "finalBoneMatrices[1]" ...
                    std::string uniformName = "finalBoneMatrices[" + std::to_string(i) + "]";
                    m_skinningProgram->SetUniform(uniformName, finalMatrices[i]);
                }

                m_model->Draw(m_skinningProgram.get());
            }
        }
    }
   
    IMGUI.EndFrame();
}

bool Context::Init()
{
    // 1. 큐브 생성
    {
        m_box = Mesh::CreateBox();
    }

    // 2. 렌더링 파이프라인 생성
    {
        // 1. 기본 텍스쳐 프로그램 생성
        {
            m_program = Program::Create
            (
                "./Resources/Shaders/texture.vert", 
                "./Resources/Shaders/texture.frag"
            );
            if (!m_program) return false;
            SPDLOG_INFO("program id: {}", m_program->Get());
        }

        // 2. 조명의 영향을 받는 단색 텍스쳐 프로그램 생성
        {
            m_simpleProgram = Program::Create
            (
                "./Resources/Shaders/simple.vert",
                "./Resources/Shaders/simple.frag"
            );
        }

        // 3. 조명 프로그램 생성
        {
            m_lighting = Program::Create
            (
                "./Resources/Shaders/lighting.vert", 
                "./Resources/Shaders/lighting.frag"
            );
            if (!m_lighting) return false;
            SPDLOG_INFO("program id: {}", m_lighting->Get());
        }

        // 4. 텍스쳐에 조명 효과
        {
            m_lighting2 = Program::Create
            (
                "./Resources/Shaders/lighting2.vert",
                "./Resources/Shaders/lighting2.frag"
            );
        }

        // 5. 모델 셰이더
        {
            m_skinningProgram = Program::Create(
                "./Resources/Shaders/skinning.vert",
                "./Resources/Shaders/skinning.frag"
            );
            if (!m_skinningProgram) return false;
        }

    } glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // 이미지 로드
    {
        // 블록 나무 이미지 로드 후 텍스쳐 생성
        auto image1 = Image::Load("./Resources/Images/container.jpg");
        if (!image1)  return false;
        m_texture1 = Texture::CreateFromImage(image1.get());

        // awesomeface 이미지 로드 후 텍스쳐 생성 (복수 이미지 로드)
        auto image2 = Image::Load("./Resources/Images/awesomeface.png");
        if (!image2)  return false;
        m_texture2 = Texture::CreateFromImage(image2.get());

        // Material #2를 위한 머티리얼 맵들을 생성
        /*auto image3 = Image::Load("./Resources/Images/container2.png");
        if (!image3)  return false;
        m_material.diffuse = Texture::CreateFromImage(image3.get());


        auto image4 = Image::Load("./Resources/Images/container2_specular.png");
        if (!image4)  return false;
        m_material.specular = Texture::CreateFromImage(image4.get());*/

        m_material.diffuse = Texture::CreateFromImage(
            Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());

        m_material.specular = Texture::CreateFromImage(
            Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
            
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture1->Get());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

        m_program->Use();
        m_program->SetUniform("tex", 0);
        m_program->SetUniform("tex2", 1);
    }

    // 큐브의 Transform과 카메라 생성
    {
        // 첫 번째 큐브 Transform
        m_cubeTransform1 = Transform::Create();
        m_cubeTransform1->SetPosition(glm::vec3(5.0f, 10.0f, 0.0f));

        // 두 번째 큐브 Transform
        m_cubeTransform2 = Transform::Create();
        m_cubeTransform2->SetPosition(glm::vec3(2.0f, 2.0f, 0.0f));

        // 조명
        m_pointLight = PointLight::Create();
        m_pointLight->GetTransform().SetPosition(glm::vec3(3.0f, 3.0f, 3.0f));

        m_directionalLight = DirectionalLight::Create();

        m_spotLight = SpotLight::Create();
        m_spotLight->GetTransform().SetPosition(glm::vec3(3.0f, 3.0f, 3.0f));

        // 카메라
        m_camera = Camera::Create();
        m_camera->GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
        m_camera->SetProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
            0.01f, 100.0f);

        // 모델
        m_model = Model::Load("./Resources/Models/spacesoldier/aliensoldier.fbx");
        if (!m_model) return false;
        m_modelTransform = Transform::Create();
        m_modelTransform->SetScale(glm::vec3(0.01f));
        {
            auto animation = Animation::Create(
                "./Resources/Models/spacesoldier/Idle.fbx", m_model.get());
            if (!animation)
            {
                SPDLOG_ERROR("Failed to load animation");
                return false;
            }

            // 애니메이터 생성
            m_animator = Animator::Create(std::move(animation));
            if (!m_animator) return false;
        }

    }

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
