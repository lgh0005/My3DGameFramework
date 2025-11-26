#include "EnginePch.h"
#include "Context.h"

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

    // 2. ImGui 렌더링 (Context가 담당?)
    {
        IMGUI.BeginFrame();
        {
            // imgui context #1 : 카메라
            if (IMGUI.Begin("Camera Parameters"))
            {
                ImGui::Text("TODO : Move to content side later..");
                //if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
                //    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

                //ImGui::Separator();

                //// [수정] m_camera가 유효한지 확인하고 GetOwner() 사용
                //if (m_camera)
                //{
                //    auto& camTransform = m_camera->GetTransform();
                //    glm::vec3 cameraPos = camTransform.GetPosition();
                //    bool posChanged = ImGui::DragFloat3("camera pos", glm::value_ptr(cameraPos), 0.01f);
                //    bool rotChanged = ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
                //    rotChanged |= ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);

                //    if (posChanged) camTransform.SetPosition(cameraPos);
                //    if (rotChanged) camTransform.SetRotation(glm::vec3(m_cameraPitch, m_cameraYaw, 0.0f));

                //    ImGui::Separator();
                //    if (ImGui::Button("reset camera"))
                //    {
                //        m_cameraYaw = 0.0f;
                //        m_cameraPitch = 0.0f;
                //        camTransform.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
                //        camTransform.SetRotation(glm::vec3(m_cameraPitch, m_cameraYaw, 0.0f));
                //    }
                //}
            } IMGUI.End();

            // imgui context #2 : 조명
            if (IMGUI.Begin("Light Parameters"))
            {
                ImGui::Text("TODO : Move to content side later..");

                /*if (m_spotLight)
                {
                    auto& lightTransform = m_spotLight->GetTransform();
                    glm::vec3 lightPos = lightTransform.GetPosition();
                    if (ImGui::DragFloat3("position", glm::value_ptr(lightPos), 0.01f))
                        lightTransform.SetPosition(lightPos);

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
                }*/
            } IMGUI.End();
        }

        if (IMGUI.Begin("Gamma correction"))
        {
            ImGui::Text("TODO : Move to content side later..");
        } IMGUI.End();

        IMGUI.EndFrame();
    }
}

bool Context::Init()
{
    SCENE.LoadScene("DevScene");

    // [수정] 씬 로드 및 ImGui 포인터 캐시
    //Scene* activeScene = SCENE.GetActiveScene();
    //if (activeScene)
    //{
    //    // m_camera = activeScene->GetMainCamera();
    //    m_spotLight = (SpotLight*)activeScene->GetMainLight();
    //}

    return true;
}