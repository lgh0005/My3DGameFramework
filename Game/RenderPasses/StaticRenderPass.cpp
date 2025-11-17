#include "pch.h"
#include "StaticRenderPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Core/GameObject.h"

StaticRenderPassUPtr StaticRenderPass::Create(ProgramUPtr program)
{
    auto pass = StaticRenderPassUPtr(new StaticRenderPass());
    if (!pass->Init(std::move(program))) return nullptr;
    return std::move(pass);
}

void StaticRenderPass::Render(Scene* scene, Camera* camera)
{
    // 1. 셰이더 활성화
    m_program->Use();

    // 2. 패스 공통 유니폼 설정 (View, Projection, CameraPos)
    auto projection = camera->GetProjectionMatrix();
    auto view = camera->GetViewMatrix();
    auto cameraPos = camera->GetTransform().GetPosition();
    m_program->SetUniform("projection", projection);
    m_program->SetUniform("view", view);
    m_program->SetUniform("viewPos", cameraPos);

    // 3. 조명 유니폼 설정 (DevRenderer의 if(mainLight) 로직)
    SpotLight* mainLight = static_cast<SpotLight*>(scene->GetMainLight());
    if (mainLight)
    {
        auto& lightTransform = mainLight->GetTransform();
        glm::vec3 lightPos = lightTransform.GetPosition();
        glm::vec3 lightDir = lightTransform.GetForwardVector();
        glm::vec2 cutoff = mainLight->GetCutoff();
        cutoff = glm::vec2(cosf(glm::radians(cutoff[0])), cosf(glm::radians(cutoff[0] + cutoff[1])));
        glm::vec3 attenuation = Utils::GetAttenuationCoeff(mainLight->GetDistance());
        glm::vec3 ambient = mainLight->GetAmbient();
        glm::vec3 diffuse = mainLight->GetDiffuse();
        glm::vec3 specular = mainLight->GetSpecular();

        m_program->SetUniform("light.position", lightPos);
        m_program->SetUniform("light.direction", lightDir);
        m_program->SetUniform("light.cutoff", cutoff);
        m_program->SetUniform("light.attenuation", attenuation);
        m_program->SetUniform("light.ambient", ambient);
        m_program->SetUniform("light.diffuse", diffuse);
        m_program->SetUniform("light.specular", specular);
    }

    for (const auto* renderer : m_renderers)
    {
        GameObject* go = renderer->GetOwner();
        MeshPtr mesh = renderer->GetMesh();
        auto material = renderer->GetMaterial();
        auto& transform = go->GetTransform();

        material->SetToProgram(m_program.get());

        auto modelTransform = transform.GetModelMatrix();
        auto mvp = projection * view * modelTransform;
        m_program->SetUniform("transform", mvp);
        m_program->SetUniform("modelTransform", modelTransform);

        mesh->Draw(m_program.get());
    }
}