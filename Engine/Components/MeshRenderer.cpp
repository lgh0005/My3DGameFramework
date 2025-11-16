#include "EnginePch.h"
#include "MeshRenderer.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/Material.h"

MeshRendererUPtr MeshRenderer::Create(MeshPtr mesh, MaterialPtr material)
{
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
