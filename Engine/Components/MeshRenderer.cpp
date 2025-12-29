#include "EnginePch.h"
#include "MeshRenderer.h"

#include "Components/Transform.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

MeshRenderer::MeshRenderer() = default;
MeshRenderer::~MeshRenderer() = default;

RenderBounds MeshRenderer::GetWorldBounds() const
{
    if (!m_mesh) return RenderBounds::Empty();
    return m_mesh->GetLocalBounds().Transform(GetTransform().GetWorldMatrix());
}