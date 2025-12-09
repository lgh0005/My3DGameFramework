#include "pch.h"
#include "SphericalToCubePass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Core/GameObject.h"

SphericalToCubePass::SphericalToCubePass() = default;
SphericalToCubePass::~SphericalToCubePass() = default;

SphericalToCubePassUPtr SphericalToCubePass::Create(ProgramUPtr program)
{
    auto pass = SphericalToCubePassUPtr(new SphericalToCubePass());
    if (!pass->Init(std::move(program))) return nullptr;
    return std::move(pass);
}

bool SphericalToCubePass::Init(ProgramUPtr program)
{
    m_sphericalToCubeProgram = std::move(program);
    if (!m_sphericalToCubeProgram) return false;
    return true;
}

void SphericalToCubePass::Render(Scene* scene, Camera* camera)
{
    m_sphericalToCubeProgram->Use();

    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix();

    for (const auto* renderer : m_renderers)
    {
        GameObject* go = renderer->GetOwner();
        MeshPtr mesh = renderer->GetMesh();
        auto material = renderer->GetMaterial();
        auto& transform = go->GetTransform();
        material->SetToProgram(m_sphericalToCubeProgram.get());

        // TODO : 유니폼 변수 전송
        glm::mat4 model = go->GetTransform().GetModelMatrix();
        glm::mat4 mvp = projection * view * model;
        m_sphericalToCubeProgram->SetUniform("transform", mvp);

        mesh->Draw(m_sphericalToCubeProgram.get());
    }
}

const std::vector<MeshRenderer*>& SphericalToCubePass::GetRenderers() const
{
    return m_renderers;
}

void SphericalToCubePass::AddRenderer(MeshRenderer* meshRenderer)
{
    m_renderers.push_back(meshRenderer);
}
