#include "pch.h"
#include "TestRenderPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Core/GameObject.h"

TestRenderPassUPtr TestRenderPass::Create(ProgramUPtr program)
{
    auto pass = TestRenderPassUPtr(new TestRenderPass());
    if (!pass->Init(std::move(program))) return nullptr;
    return std::move(pass);
}

bool TestRenderPass::Init(ProgramUPtr program)
{
    m_simpleProgram = std::move(program);
    if (!m_simpleProgram) return false;

    // [초기화] 텍스처 슬롯 번호는 변하지 않으므로 한 번만 세팅
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("material.albedoMap", (int)TextureSlot::SLOT_ALBEDO);
    m_simpleProgram->SetUniform("material.normalMap", (int)TextureSlot::SLOT_NORMAL);
    m_simpleProgram->SetUniform("material.metallicMap", (int)TextureSlot::SLOT_METALLIC);
    m_simpleProgram->SetUniform("material.roughnessMap", (int)TextureSlot::SLOT_ROUGHNESS);
    m_simpleProgram->SetUniform("material.aoMap", (int)TextureSlot::SLOT_AO);

    return true;
}

void TestRenderPass::Render(Scene* scene, Camera* camera)
{
    m_simpleProgram->Use();

    for (const auto* renderer : m_renderers)
    {
        GameObject* go = renderer->GetOwner();
        MeshPtr mesh = renderer->GetMesh();
        auto& transform = go->GetTransform();
        auto material = renderer->GetMaterial();
        material->SetToProgram(m_simpleProgram.get());

        // 2. 모델 행렬 전송
        auto modelMatrix = transform.GetModelMatrix();
        m_simpleProgram->SetUniform("model", modelMatrix);

        mesh->Draw(m_simpleProgram.get());
    }
}

const std::vector<MeshRenderer*>& TestRenderPass::GetRenderers() const
{
    return m_renderers;
}

void TestRenderPass::AddRenderer(MeshRenderer* meshRenderer)
{
    m_renderers.push_back(meshRenderer);
}
