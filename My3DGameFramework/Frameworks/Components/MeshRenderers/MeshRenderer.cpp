#include "FrameworkPch.h"
#include "MeshRenderer.h"
#include "Meshes/StaticMesh.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	MeshRenderer::MeshRenderer
	(
		ObjectIDHash id, ObjectIDHash ownerID,
		const MeshPtr& mesh, const MaterialPtr& material
	)
		: Super(id, ownerID), m_mesh(mesh), m_material(material) { }
	MeshRenderer::~MeshRenderer() = default;
	MeshRenderer::MeshRenderer(MeshRenderer&& other) noexcept = default;
	MeshRenderer& MeshRenderer::operator=(MeshRenderer&& other) noexcept = default;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	int16 MeshRenderer::s_typeIndex = -1;
	const MGFType* MeshRenderer::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	StaticMesh* MeshRenderer::GetMesh() const
	{
		return MGFTypeCaster::Cast<StaticMesh*>(m_mesh.get());
	}

	RenderBounds MeshRenderer::GetLocalBounds() const
	{
		return m_mesh ? m_mesh->GetLocalBounds() : RenderBounds::Empty();
	}
}