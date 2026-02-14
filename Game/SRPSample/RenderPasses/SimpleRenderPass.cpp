#include "pch.h"
#include "SimpleRenderPass.h"

#include "Scene/Scene.h"
#include "Resources/Programs/Program.h"
#include "Resources/Meshes/Mesh.h"
#include "Resources/Material.h"
#include "Components/Camera.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Object/GameObject.h"

DECLARE_DEFAULTS_IMPL(SimpleRenderPass)

SimpleRenderPassUPtr SimpleRenderPass::Create(ProgramPtr program)
{
    auto pass = SimpleRenderPassUPtr(new SimpleRenderPass());
    if (!pass->Init(std::move(program))) return nullptr;
    return std::move(pass);
}

bool SimpleRenderPass::Init(ProgramPtr program)
{
    m_simpleProgram = std::move(program);
    if (!m_simpleProgram) return false;
    return true;
}

void SimpleRenderPass::Render(Scene* scene, Camera* camera)
{
    m_simpleProgram->Use();

    //// TOOD : 이제 이것 역시도 UBO로 대체 할 필요 있음
    auto projection = camera->GetProjectionMatrix();
    auto view = camera->GetViewMatrix();

    for (const auto* renderer : m_renderers)
    {
        GameObject* go = renderer->GetOwner();
        auto& transform = go->GetTransform();

        auto lightModel = transform.GetWorldMatrix();
        auto lightMvp = projection * view * lightModel;
        m_simpleProgram->SetUniform("transform", lightMvp);

        // 색상을 하얀색(1.0, 1.0, 1.0, 1.0)으로 고정
        m_simpleProgram->SetUniform("color", glm::vec4(1.0f));

        renderer->Render(m_simpleProgram.get());
    }
}

const std::vector<StaticMeshRenderer*>& SimpleRenderPass::GetRenderers() const
{
    return m_renderers;
}

void SimpleRenderPass::AddRenderer(StaticMeshRenderer* meshRenderer)
{
    m_renderers.push_back(meshRenderer);
}
