#include "EnginePch.h"
#include "StaticMeshRenderer.h"
#include "Resources/StaticMesh.h"
#include "Components/Transform.h"

StaticMeshRenderer::StaticMeshRenderer() = default;
StaticMeshRenderer::~StaticMeshRenderer() = default;

StaticMeshRendererUPtr StaticMeshRenderer::Create(StaticMeshPtr mesh, MaterialPtr material)
{
	auto renderer = StaticMeshRendererUPtr(new StaticMeshRenderer());
	if (!renderer->Init(mesh, material)) return nullptr;
	return std::move(renderer);
}

bool StaticMeshRenderer::Init(StaticMeshPtr mesh, MaterialPtr material)
{
	m_mesh = mesh;
	m_material = material;
	if (!m_mesh || !m_material) return false;
	return true;
}

RenderBounds StaticMeshRenderer::GetWorldBounds() const
{
	Super::GetWorldBounds();

    // 1. 기본 AABB는 T-Pose AABB를 사용.
    RenderBounds bounds = m_mesh->GetLocalBounds();

    // 2. Transform의 최신 행렬 적용
    return bounds.Transform(GetTransform().GetWorldMatrix());
}
