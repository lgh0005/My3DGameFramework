#include "EnginePch.h"
#include "InstancedMeshRenderer.h"
#include "Resources/InstancedMesh.h"

InstancedMeshRenderer::InstancedMeshRenderer() = default;
InstancedMeshRenderer::~InstancedMeshRenderer() = default;

InstancedMeshRendererUPtr InstancedMeshRenderer::Create(InstancedMeshPtr mesh, MaterialPtr material)
{
	auto renderer = InstancedMeshRendererUPtr(new InstancedMeshRenderer());
	if (!renderer->Init(mesh, material)) return nullptr;
	return std::move(renderer);
}

bool InstancedMeshRenderer::Init(InstancedMeshPtr mesh, MaterialPtr material)
{
	m_mesh = mesh;
	m_material = material;
	if (!m_mesh || !m_material) return false;
	return true;
}

RenderBounds InstancedMeshRenderer::GetWorldBounds() const
{
	return RenderBounds::Empty();
}
