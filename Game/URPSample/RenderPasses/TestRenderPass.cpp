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
    return true;
}

void TestRenderPass::Render(Scene* scene, Camera* camera)
{
    m_simpleProgram->Use();

    //// TOOD : 이제 이것 역시도 UBO로 대체 할 필요 있음
    auto projection = camera->GetProjectionMatrix();
    auto view = camera->GetViewMatrix();

    for (const auto* renderer : m_renderers)
    {
        GameObject* go = renderer->GetOwner();
        MeshPtr mesh = renderer->GetMesh();
        auto& transform = go->GetTransform();
        auto material = renderer->GetMaterial();

        material->SetToProgram(m_simpleProgram.get());

        // 위치 설정
        auto lightModel = transform.GetModelMatrix();
        auto lightMvp = projection * view * lightModel;
        m_simpleProgram->SetUniform("transform", lightMvp);

        // 색상 설정
        m_simpleProgram->SetUniform("color", glm::vec4(0.5, 0.0f, 1.0, 0.7f));

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
