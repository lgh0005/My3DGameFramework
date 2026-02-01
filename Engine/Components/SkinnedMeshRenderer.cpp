#include "EnginePch.h"
#include "SkinnedMeshRenderer.h"
#include "Object/GameObject.h"
#include "Components/Animator.h"
#include "Components/Transform.h"
#include "Resources/Meshes/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Resources/Programs/Program.h"
#include "Graphics/Layouts/VertexLayout.h"

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

    // TODO : 안정성을 위해서 폴백 로직 추가 고려
    
    //// 1. 이미 캐싱된 게 있으면 즉시 리턴 (Fast Path)
    //if (m_referenceAnimator) return m_referenceAnimator;

    //// 2. 캐싱된 게 없다면? 계층 구조를 뒤져서 찾아본다. (Slow Path - 최초 1회만 발생)

    //// A. 내 게임 오브젝트(Owner)에 Animator가 있는지 확인
    //GameObject* owner = GetOwner();
    //if (!owner) return nullptr;

    //m_referenceAnimator = owner->GetComponent<Animator>();
    //if (m_referenceAnimator) return m_referenceAnimator;

    //// B. 내 부모들을 타고 올라가면서 확인 (보통 루트에 있음)
    //// 간단하게 GetRoot()를 사용해서 루트의 Animator를 확인
    //// (모델 구조상 보통 루트에 Animator가 붙기 때문)
    //GameObject* root = const_cast<GameObject*>(owner->GetRoot());
    //if (root) m_referenceAnimator = root->GetComponent<Animator>();

    //return m_referenceAnimator;
}

void SkinnedMeshRenderer::Render(Program* program) const
{
    if (m_material) m_material->SetToProgram(program);
    m_mesh->Draw();
}
