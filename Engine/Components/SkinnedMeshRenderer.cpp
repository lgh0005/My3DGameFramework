#include "EnginePch.h"
#include "SkinnedMeshRenderer.h"
#include "Scene/GameObject.h"
#include "Components/Animator.h"
#include "Components/Transform.h"
#include "Resources/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Resources/Program.h"
#include "Graphics/VertexLayout.h"

DECLARE_DEFAULTS_IMPL(SkinnedMeshRenderer)

SkinnedMeshRendererUPtr SkinnedMeshRenderer::Create(SkinnedMeshPtr mesh, MaterialPtr material, Animator* animator)
{
	auto renderer = SkinnedMeshRendererUPtr(new SkinnedMeshRenderer());
	if (!renderer->Init(mesh, material, animator)) return nullptr;
	return std::move(renderer);
}

bool SkinnedMeshRenderer::Init(SkinnedMeshPtr mesh, MaterialPtr material, Animator* animator)
{
	m_mesh = mesh;
	m_material = material;
    m_referenceAnimator = animator;
	if (!m_mesh || !m_material) return false;
	return true;
}

RenderBounds SkinnedMeshRenderer::GetWorldBounds() const
{
    if (!m_mesh) return RenderBounds::Empty();

    // 1. 기본 AABB는 T-Pose AABB를 사용.
    RenderBounds bounds = m_mesh->GetLocalBounds();

    // 2. Animator의 최신 AABB와 Union
    auto* animator = GetAnimator();
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
    return m_referenceAnimator;
}

void SkinnedMeshRenderer::Render(Program* program) const
{
    if (m_material) m_material->SetToProgram(program);
    m_mesh->Draw();
}
