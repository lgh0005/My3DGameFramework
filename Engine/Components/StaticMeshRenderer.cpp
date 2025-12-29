#include "EnginePch.h"
#include "StaticMeshRenderer.h"
#include "Resources/StaticMesh.h"
#include "Resources/Material.h"
#include "Graphics/Program.h"
#include "Graphics/VertexLayout.h"
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
	return Super::GetWorldBounds();
}

void StaticMeshRenderer::Render(Program* program) const
{
	if (m_material) m_material->SetToProgram(program);
	m_mesh->Draw();
}
