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

void Context::Render()
{
    // 프레임 버퍼 기반 렌더링 로직
    {
        // 프레임 버퍼 바인딩
        m_frameBuffer->Bind();

        // render context
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

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
            m_lighting2->SetUniform
            (
                "light.cutoff", glm::vec2
                (
                    cosf(glm::radians(cutoff[0])),
                    cosf(glm::radians(cutoff[0] + cutoff[1]))
                )
            );

            m_lighting2->SetUniform("light.attenuation", Utils::GetAttenuationCoeff(m_spotLight->GetDistance()));
            m_lighting2->SetUniform("light.ambient", m_spotLight->GetAmbient());
            m_lighting2->SetUniform("light.diffuse", m_spotLight->GetDiffuse());
            m_lighting2->SetUniform("light.specular", m_spotLight->GetSpecular());

            m_lighting2->SetUniform("material.diffuse", 0);
            m_lighting2->SetUniform("material.specular", 1);
            glActiveTexture(GL_TEXTURE0);
            m_lightMaterial->diffuse->Bind();
            glActiveTexture(GL_TEXTURE1);
            m_lightMaterial->specular->Bind();
            m_lighting2->SetUniform("material.shininess",  m_lightMaterial->shininess);
        }

        // 큐브 물체
        {
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
                    std::string uniformName = "finalBoneMatrices[" + std::to_string(i) + "]";
                    m_skinningProgram->SetUniform(uniformName, finalMatrices[i]);
                }

                m_model->Draw(m_skinningProgram.get());
                m_animator->UpdateAnimation();
            }
            
            // 큐브와 바닥
            {
                // 셰이더 재설정
                m_lighting2->Use();

                // 1. 바닥 그리기
                auto modelTransform = m_groundTransform->GetModelMatrix();
                auto transform = projection * view * modelTransform;
                m_lighting2->SetUniform("transform", transform);
                m_lighting2->SetUniform("modelTransform", modelTransform);
                m_planeMaterial->SetToProgram(m_lighting2.get());
                m_box->Draw(m_lighting2.get());

                // 2. 큐브 #1 그리기
                modelTransform = m_box1Transform->GetModelMatrix();
                transform = projection * view * modelTransform;
                m_lighting2->SetUniform("transform", transform);
                m_lighting2->SetUniform("modelTransform", modelTransform);
                m_box1Material->SetToProgram(m_lighting2.get());
                m_box->Draw(m_lighting2.get());

                // 외곽선이 있는 큐브
                {
                    glEnable(GL_STENCIL_TEST);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

                    // 스텐실 테스트는 무조건 성공
                    glStencilFunc(GL_ALWAYS, 1, 0xFF); 
                    glDepthMask(GL_FALSE);
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                    {
                        // 3. 큐브 #2 그리기
                        modelTransform = m_box2Transform->GetModelMatrix();
                        transform = projection * view * modelTransform;
                        m_lighting2->SetUniform("transform", transform);
                        m_lighting2->SetUniform("modelTransform", modelTransform);
                        m_box2Material->SetToProgram(m_lighting2.get());
                        m_box->Draw(m_lighting2.get());
                    }

                    // 단색 외곽선 큐브
                    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                    glStencilMask(0x00); // 스텐실 버퍼 쓰기 끄기
                    glDepthMask(GL_TRUE); // 깊이 버퍼 쓰기/테스트 다시 켜기
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // 컬러 버퍼 쓰기 다시 켜기
                    glEnable(GL_DEPTH_TEST);

                    m_simpleProgram->Use();
                    m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                    m_simpleProgram->SetUniform("transform",
                        transform* glm::scale(glm::mat4(1.0f), glm::vec3(1.02f, 1.02f, 1.02f)));
                    m_box->Draw(m_simpleProgram.get());

                    glStencilFunc(GL_EQUAL, 1, 0xFF);

                    m_lighting2->Use(); // 셰이더 원복
                    m_lighting2->SetUniform("transform", transform); // (Pass 1에서 쓴 값 재사용)
                    m_lighting2->SetUniform("modelTransform", modelTransform); // (Pass 1에서 쓴 값 재사용)
                    m_box2Material->SetToProgram(m_lighting2.get());
                    m_box->Draw(m_lighting2.get());

                    // --- 4. 상태 복구 ---
                    glDisable(GL_STENCIL_TEST);
                    glStencilMask(0xFF); // 기본값으로
                    glStencilFunc(GL_ALWAYS, 1, 0xFF); // 기본값으로
                    // glDepthMask, glColorMask는 이미 TRUE로 복구됨
                }
            }
        }
        
        // 화면을 위에서 그린 내용으로 전환
        Framebuffer::BindToDefault();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        m_postProgram->Use();
        m_postProgram->SetUniform("transform", glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));
        m_postProgram->SetUniform("gamma", m_gamma);
        m_frameBuffer->GetColorAttachment()->Bind();
        m_postProgram->SetUniform("tex", 0);
        m_plane->Draw(m_postProgram.get());
    }

    // imgui 컨텍스트들
    {
        IMGUI.BeginFrame();

        {
            // imgui context #1 : 카메라에 대한 imgui 창.
            if (IMGUI.Begin("Camera Parameters"))
            {
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
        }

        // imgui context #3 : 포스트-프로세싱 imgui 창.
        if (IMGUI.Begin("Gamma correction"))
        {
            ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
        } IMGUI.End();

        IMGUI.EndFrame();
    }
}

bool Context::Init()
{
    // 1. 큐브 생성
    {
        m_box = Mesh::CreateBox();
        m_plane = Mesh::CreatePlane();
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
            m_skinningProgram = Program::Create
            (
                "./Resources/Shaders/skinning.vert",
                "./Resources/Shaders/skinning.frag"
            );
            if (!m_skinningProgram) return false;
        }

        // 6. 포스트-프로세싱 셰이더
        {
            m_postProgram = Program::Create
            (
                "./Resources/Shaders/PostProcessing/postprocess.vert",
                "./Resources/Shaders/PostProcessing/postprocess.frag"
            );
            if (!m_postProgram) return false;
        }
    
    } glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // 3. 프레임 버퍼 생성
    {
        m_frameBuffer = Framebuffer::Create(Texture::Create(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA));
        if (!m_frameBuffer) false;
    }

    // 이미지 로드
    {
        TexturePtr darkGrayTexture = Texture::CreateFromImage
        (Image::CreateSingleColorImage(4, 4, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());
        TexturePtr grayTexture = Texture::CreateFromImage
        (Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());

        m_planeMaterial = Material::Create();
        m_planeMaterial->diffuse = Texture::CreateFromImage(Image::Load("./Resources/Images/marble.jpg").get());
        m_planeMaterial->specular = grayTexture;
        m_planeMaterial->shininess = 128.0f;

        m_box1Material = Material::Create();
        m_box1Material->diffuse = Texture::CreateFromImage(Image::Load("./Resources/Images/container.jpg").get());
        m_box1Material->specular = darkGrayTexture;
        m_box1Material->shininess = 16.0f;

        m_box2Material = Material::Create();
        m_box2Material->diffuse = Texture::CreateFromImage(Image::Load("./Resources/Images/container2.png").get());
        m_box2Material->specular = Texture::CreateFromImage(Image::Load("./Resources/Images/container2_specular.png").get());
        m_box2Material->shininess = 64.0f;

        m_lightMaterial = Material::Create();
        m_lightMaterial->diffuse = Texture::CreateFromImage
        (Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());
        m_lightMaterial->specular = Texture::CreateFromImage
        (Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
    }

    // 큐브의 Transform과 카메라 생성
    {
        // 조명
        m_pointLight = PointLight::Create();
        m_pointLight->GetTransform().SetPosition(glm::vec3(3.0f, 3.0f, 3.0f));

        m_directionalLight = DirectionalLight::Create();

        m_spotLight = SpotLight::Create();
        m_spotLight->GetTransform().SetPosition(glm::vec3(1.0f, 4.0f, 4.0f));
        m_spotLight->SetCutoff(glm::vec2(120.0f, 5.0f));
        m_spotLight->SetDistance(128.0f);

        // 카메라
        m_camera = Camera::Create();
        m_camera->GetTransform().SetPosition(glm::vec3(0.0f, 2.5f, 8.0f));
        m_camera->SetProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 100.0f);

        // IMPORTANT : 기본적인 Scene UI를 그릴 때 는 DEPTH_TEST를 꺼야한다.

        // 모델 #1 
        m_model = Model::Load("./Resources/Models/spacesoldier/aliensoldier.mymodel");
        if (!m_model) return false;
        m_modelTransform = Transform::Create();
        m_modelTransform->SetPosition(glm::vec3(2.0f, 0.0f, -2.0f));
        m_modelTransform->SetScale(glm::vec3(0.025f));
        {
            auto animation = Animation::Load(
                "./Resources/Models/spacesoldier/Hip Hop Dancing.fbx", m_model.get());
            if (!animation)
            {
                SPDLOG_ERROR("Failed to load animation");
                return false;
            }

            // 애니메이터 생성
            m_animator = Animator::Create(std::move(animation));
            if (!m_animator) return false;
        }
    
        // 박스와 바닥
        m_box1Transform = Transform::Create();
        m_box1Transform->SetPosition(glm::vec3(-1.0f, 0.75f, -4.0f));
        m_box1Transform->SetRotation(glm::vec3(0.0f, 30.0f, 0.0f));
        m_box1Transform->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

        m_box2Transform = Transform::Create();
        m_box2Transform->SetPosition(glm::vec3(0.0f, 0.75f, 2.0f));
        m_box2Transform->SetRotation(glm::vec3(0.0f, 20.0f, 0.0f));
        m_box2Transform->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

        m_groundTransform = Transform::Create();
        m_groundTransform->SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));
        m_groundTransform->SetScale(glm::vec3(10.0f, 1.0f, 10.0f));
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

#pragma endregion