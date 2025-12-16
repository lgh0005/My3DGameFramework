#include "pch.h"
#include "InstancedRenderPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Core/GameObject.h"

InstancedRenderPassUPtr InstancedRenderPass::Create(ProgramUPtr program)
{
    auto pass = InstancedRenderPassUPtr(new InstancedRenderPass());
    if (!pass->Init(std::move(program))) return nullptr;
    return std::move(pass);
}

bool InstancedRenderPass::Init(ProgramUPtr program)
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
        MeshPtr mesh = renderer->GetMesh();
        auto material = renderer->GetMaterial();
        if (!mesh || !material) continue;

        material->SetToProgram(m_instanceProgram.get());

        // (셰이더가 aInstanceOffset으로 직접 위치를 계산)
        auto& transform = renderer->GetTransform();
        m_instanceProgram->SetUniform("model", transform.GetWorldMatrix());

        mesh->Draw(m_instanceProgram.get());
    }
}

const std::vector<MeshRenderer*>& InstancedRenderPass::GetRenderers() const
{
    return m_renderers;
}

void InstancedRenderPass::AddRenderer(MeshRenderer* meshRenderer)
{
    m_renderers.push_back(meshRenderer);
}