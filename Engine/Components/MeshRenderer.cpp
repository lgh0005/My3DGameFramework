#include "EnginePch.h"
#include "MeshRenderer.h"

#include "Core/GameObject.h"
#include "Components/Animator.h"
#include "Components/Transform.h"
#include "Resources/Mesh.h"
#include "Resources/StaticMesh.h"
#include "Resources/SkinnedMesh.h"
#include "Resources/Material.h"

RenderBounds MeshRenderer::GetWorldBounds() const
{
    if (!m_mesh) return RenderBounds();
}