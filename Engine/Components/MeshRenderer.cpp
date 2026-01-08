#include "EnginePch.h"
#include "MeshRenderer.h"

#include "Components/Transform.h"
#include "Resources/Program.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

DECLARE_DEFAULTS_IMPL(MeshRenderer)

bool MeshRenderer::MatchesType(ComponentType type) const
{
    return type == ComponentType::MeshRenderer || Super::MatchesType(type);
}

RenderBounds MeshRenderer::GetWorldBounds() const
{
    if (!m_mesh) return RenderBounds::Empty();
    return m_mesh->GetLocalBounds().Transform(GetTransform().GetWorldMatrix());
}