#include "pch.h"
#include "SimpleRenderPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Core/GameObject.h"

SimpleRenderPassUPtr SimpleRenderPass::Create(ProgramUPtr program)
{
    auto pass = SimpleRenderPassUPtr(new SimpleRenderPass());
    if (!pass->Init(std::move(program))) return nullptr;
    return std::move(pass);
}

void SimpleRenderPass::Render(Scene* scene, Camera* camera)
{
    m_program->Use();
    auto projection = camera->GetProjectionMatrix();
    auto view = camera->GetViewMatrix();
    m_program->SetUniform("projection", projection);
    m_program->SetUniform("view", view);

    for (const auto* renderer : m_renderers)
    {
        GameObject* go = renderer->GetOwner();
        MeshPtr mesh = renderer->GetMesh();
        auto& transform = go->GetTransform();
        auto material = renderer->GetMaterial();

        material->SetToProgram(m_program.get());

        auto lightModel = transform.GetModelMatrix();
        auto lightMvp = projection * view * lightModel;
        m_program->SetUniform("transform", lightMvp);

        // 색상을 하얀색(1.0, 1.0, 1.0, 1.0)으로 고정
        m_program->SetUniform("color", glm::vec4(1.0f));

        mesh->Draw(m_program.get());
    }
}
