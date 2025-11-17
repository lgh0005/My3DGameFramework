#include "pch.h"
#include "SkinningRenderPass.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"


SkinningRenderPassUPtr SkinningRenderPass::Create(ProgramUPtr program)
{
    auto pass = SkinningRenderPassUPtr(new SkinningRenderPass());
    if (!pass->Init(std::move(program))) return nullptr;
    return std::move(pass);
}

void SkinningRenderPass::Render(Scene* scene, Camera* camera)
{
    m_program->Use();

    auto projection = camera->GetProjectionMatrix();
    auto view = camera->GetViewMatrix();
    auto cameraPos = camera->GetTransform().GetPosition();
    m_program->SetUniform("projection", projection);
    m_program->SetUniform("view", view);
    m_program->SetUniform("viewPos", cameraPos);

    SpotLight* mainLight = static_cast<SpotLight*>(scene->GetMainLight());
    if (mainLight)
    {
        auto& lightTransform = mainLight->GetOwner()->GetTransform();
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

        Animator* animator = go->GetComponent<Animator>();
        if (!animator || !mesh || !material) continue;

        material->SetToProgram(m_program.get());

        // »À(Bone) À¯´ÏÆû ¼³Á¤
        auto finalMatrices = animator->GetFinalBoneMatrices();
        for (int i = 0; i < finalMatrices.size(); ++i)
            m_program->SetUniform("finalBoneMatrices[" + std::to_string(i) + "]", finalMatrices[i]);

        // ¸ðµ¨ º¯È¯ Çà·Ä
        m_program->SetUniform("model", transform.GetModelMatrix());

        mesh->Draw(m_program.get());
    }
}
