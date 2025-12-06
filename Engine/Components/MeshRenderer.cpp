#include "EnginePch.h"
#include "MeshRenderer.h"

#include "Core/GameObject.h"
#include "Components/Animator.h"
#include "Components/Transform.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/SkinnedMesh.h"
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

RenderBounds MeshRenderer::GetWorldBounds() const
{
    if (!m_mesh) return RenderBounds();

    // 1. 기본 AABB는 T-Pose AABB를 사용.
    RenderBounds bounds = m_mesh->GetLocalBounds();

    // 2. Animator의 최신 AABB와 Union
    auto* animator = GetOwner()->GetComponent<Animator>();
    if (animator)
    {
        // Animator가 Update()에서 계산한 최신 포즈의 AABB를 가져와 Union
        const RenderBounds& dynamicBounds = animator->GetCurrentLocalBounds();
        bounds = bounds.Union(dynamicBounds);
    }

    // 3. Transform의 최신 행렬 적용
    return bounds.Transform(GetTransform().GetModelMatrix());
}