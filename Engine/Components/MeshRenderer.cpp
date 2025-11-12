#include "EnginePch.h"
#include "MeshRenderer.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"

MeshRendererUPtr MeshRenderer::Create(MeshPtr mesh, MaterialPtr material)
{
    // private 생성자를 호출하여 unique_ptr로 감쌉니다.
    auto meshRenderer = MeshRendererUPtr(new MeshRenderer());
    if (!meshRenderer->Init(mesh, material)) return nullptr;
    return meshRenderer;
}

bool MeshRenderer::Init(MeshPtr mesh, MaterialPtr material)
{
    m_mesh = mesh;
    m_material = material;
    if (!m_mesh || !m_material) return false;
    return true;
}
