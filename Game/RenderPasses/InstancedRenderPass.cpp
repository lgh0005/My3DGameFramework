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

void InstancedRenderPass::Render(Scene* scene, Camera* camera)
{
    // 1. 셰이더 및 공통 유니폼
    m_program->Use();
    auto projection = camera->GetProjectionMatrix();
    auto view = camera->GetViewMatrix();
    m_program->SetUniform("projection", projection);
    m_program->SetUniform("view", view);

    for (const auto* renderer : m_renderers)
    {
        MeshPtr mesh = renderer->GetMesh();
        auto material = renderer->GetMaterial();
        if (!mesh || !material) continue;

        material->SetToProgram(m_program.get());

        // (셰이더가 aInstanceOffset으로 직접 위치를 계산)
        auto& transform = renderer->GetTransform();
        m_program->SetUniform("model", transform.GetModelMatrix());

        mesh->Draw(m_program.get());
    }
}
