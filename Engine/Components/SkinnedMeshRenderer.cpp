#include "EnginePch.h"
#include "SkinnedMeshRenderer.h"
#include "Core/GameObject.h"
#include "Components/Animator.h"
#include "Components/Transform.h"
#include "Resources/SkinnedMesh.h"

SkinnedMeshRenderer::~SkinnedMeshRenderer() = default;
SkinnedMeshRenderer::SkinnedMeshRenderer() = default;

SkinnedMeshRendererUPtr SkinnedMeshRenderer::Create(SkinnedMeshPtr mesh, MaterialPtr material)
{
	auto renderer = SkinnedMeshRendererUPtr(new SkinnedMeshRenderer());
	if (!renderer->Init(mesh, material)) return nullptr;
	return std::move(renderer);
}

bool SkinnedMeshRenderer::Init(SkinnedMeshPtr mesh, MaterialPtr material)
{
	m_mesh = mesh;
	m_material = material;
	if (!m_mesh || !m_material) return false;
	return true;
}

RenderBounds SkinnedMeshRenderer::GetWorldBounds() const
{
	Super::GetWorldBounds();

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
    return bounds.Transform(GetTransform().GetWorldMatrix());
}

Animator* SkinnedMeshRenderer::GetAnimator() const
{
    // 1. 이미 찾았으면 리턴
    if (m_cachedAnimator) return m_cachedAnimator;

    // 2. 아직 없으면 지금 찾음
    auto owner = GetOwner();
    if (owner) m_cachedAnimator = owner->GetComponentInParent<Animator>();

    return m_cachedAnimator;
}