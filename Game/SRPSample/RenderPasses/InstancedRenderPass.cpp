#include "pch.h"
#include "InstancedRenderPass.h"

#include "Scene/Scene.h"
#include "Resources/Programs/Program.h"
#include "Resources/Meshes/Mesh.h"
#include "Resources/Material.h"
#include "Components/Camera.h"
#include "Components/InstancedMeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Object/GameObject.h"

DECLARE_DEFAULTS_IMPL(InstancedRenderPass)

InstancedRenderPassUPtr InstancedRenderPass::Create(ProgramPtr program)
{
    auto pass = InstancedRenderPassUPtr(new InstancedRenderPass());
    if (!pass->Init(std::move(program))) return nullptr;
    return std::move(pass);
}

bool InstancedRenderPass::Init(ProgramPtr program)
{
    m_instanceProgram = std::move(program);
    if (!m_instanceProgram) return false;
    return true;
}

void InstancedRenderPass::Render(Scene* scene, Camera* camera)
{
    // 1. 셰이더 및 공통 유니폼
    m_instanceProgram->Use();
    auto projection = camera->GetProjectionMatrix();
    auto view = camera->GetViewMatrix();
    m_instanceProgram->SetUniform("projection", projection);
    m_instanceProgram->SetUniform("view", view);

    for (const auto* renderer : m_renderers)
    {
        // (셰이더가 aInstanceOffset으로 직접 위치를 계산)
        auto& transform = renderer->GetTransform();
        m_instanceProgram->SetUniform("model", transform.GetWorldMatrix());
        renderer->Render(m_instanceProgram.get());
    }
}

const std::vector<InstancedMeshRenderer*>& InstancedRenderPass::GetRenderers() const
{
    return m_renderers;
}

void InstancedRenderPass::AddRenderer(InstancedMeshRenderer* meshRenderer)
{
    m_renderers.push_back(meshRenderer);
}