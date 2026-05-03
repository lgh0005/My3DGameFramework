#include "FrameworkPch.h"
#include "SkinnedMeshRenderer.h"
#include "Managers/TypeManager.h"
#include "Graphics/Meshes/SkinnedMesh.h"

namespace MGF3D
{
	SkinnedMeshRenderer::SkinnedMeshRenderer
	(
		ObjectIDHash id, ObjectIDHash ownerID,
		const MeshPtr& mesh, const MaterialPtr& material
	) : Super(id, ownerID, mesh, material) { }
	SkinnedMeshRenderer::~SkinnedMeshRenderer() = default;
	SkinnedMeshRenderer::SkinnedMeshRenderer(SkinnedMeshRenderer&& other) noexcept = default;
	SkinnedMeshRenderer& SkinnedMeshRenderer::operator=(SkinnedMeshRenderer&& other) noexcept = default;

	/*===============================//
	//       Component Type          //
	//===============================*/
	int16 SkinnedMeshRenderer::s_typeIndex = -1;
	const MGFType* SkinnedMeshRenderer::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	SkinnedMesh* SkinnedMeshRenderer::GetSkinnedMesh() const
	{
		return MGFTypeCaster::Cast<SkinnedMesh*>(m_mesh.get());
	}
}