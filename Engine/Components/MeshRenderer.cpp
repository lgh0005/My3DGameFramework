#include "EnginePch.h"
#include "MeshRenderer.h"

#include "Components/Transform.h"
#include "Graphics/Program.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

DECLARE_DEFAULTS_IMPL(MeshRenderer)

RenderBounds MeshRenderer::GetWorldBounds() const
{
    if (!m_mesh) return RenderBounds::Empty();
    return m_mesh->GetLocalBounds().Transform(GetTransform().GetWorldMatrix());
}