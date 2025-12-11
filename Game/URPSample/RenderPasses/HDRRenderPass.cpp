#include "pch.h"
#include "HDRRenderPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Texture.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Core/GameObject.h"

HDRRenderPassUPtr HDRRenderPass::Create(ProgramUPtr program)
{
    auto pass = HDRRenderPassUPtr(new HDRRenderPass());
    if (!pass->Init(std::move(program))) return nullptr;
    return std::move(pass);
}

bool HDRRenderPass::Init(ProgramUPtr program)
{
    m_simpleProgram = std::move(program);
    if (!m_simpleProgram) return false;

    // 텍스처 슬롯 번호는 변하지 않으므로 한 번만 세팅
    // TODO : SRP도 이렇게 수정하기. 
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("material.diffuse", (int)TextureSlot::SLOT_ALBEDO);
    m_simpleProgram->SetUniform("material.normal", (int)TextureSlot::SLOT_NORMAL);
    m_simpleProgram->SetUniform("material.metallic", (int)TextureSlot::SLOT_METALLIC);
    m_simpleProgram->SetUniform("material.roughness", (int)TextureSlot::SLOT_ROUGHNESS);
    m_simpleProgram->SetUniform("material.ao", (int)TextureSlot::SLOT_AO);
    m_simpleProgram->SetUniform("useIBL", 1);
    return true;
}

void HDRRenderPass::Render(Scene* scene, Camera* camera)
{
    m_simpleProgram->Use();

    // Irradiance 적용
    auto* irradianceMap = scene->GetIrradianceTexture();
    if (irradianceMap)
    {
        glActiveTexture(GL_TEXTURE10);
        irradianceMap->Bind();
        m_simpleProgram->SetUniform("irradianceMap", 10);
    }

    // BRDF 적용
    auto* brdf = scene->GetBRDFLookUpTexture();
    if (brdf)
    {
        glActiveTexture(GL_TEXTURE11);
        brdf->Bind();
        m_simpleProgram->SetUniform("brdf", 11);
    }

    // 모델 그리기
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

const std::vector<MeshRenderer*>& HDRRenderPass::GetRenderers() const
{
    return m_renderers;
}

void HDRRenderPass::AddRenderer(MeshRenderer* meshRenderer)
{
    m_renderers.push_back(meshRenderer);
}
