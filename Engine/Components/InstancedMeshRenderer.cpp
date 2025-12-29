#include "EnginePch.h"
#include "InstancedMeshRenderer.h"
#include "Resources/InstancedMesh.h"
#include "Resources/Material.h"
#include "Graphics/Program.h"
#include "Graphics/VertexLayout.h"

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
	m_instancedMeshCache = mesh.get();
	m_material = material;
	if (!m_mesh || !m_material) return false;
	return true;
}

RenderBounds InstancedMeshRenderer::GetWorldBounds() const
{
	// TODO : 이후에는 월드 경계를 어떻게 해야 할 지 고민 필요
	return RenderBounds::Empty();
}

void InstancedMeshRenderer::Render(Program* program) const
{
	if (m_material) m_material->SetToProgram(program);
	m_instancedMeshCache->Draw();
}