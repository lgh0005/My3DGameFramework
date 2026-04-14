#include "FrameworkPch.h"
#include "MeshRenderer.h"
#include "Meshes/Mesh.h"
#include "Resources/Material.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	MeshRenderer::MeshRenderer(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID) { }
	MeshRenderer::~MeshRenderer() = default;
	MeshRenderer::MeshRenderer(MeshRenderer&& other) noexcept = default;
	MeshRenderer& MeshRenderer::operator=(MeshRenderer&& other) noexcept = default;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	int16 MeshRenderer::s_typeIndex = -1;
	const MGFType* MeshRenderer::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("MeshRenderer");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}


}


//DECLARE_DEFAULTS_IMPL(MeshRenderer)
//
//bool MeshRenderer::MatchesType(ComponentType type) const
//{
//    return type == ComponentType::MeshRenderer || Super::MatchesType(type);
//}
//
//RenderBounds MeshRenderer::GetWorldBounds() const
//{
//    if (!m_mesh) return RenderBounds::Empty();
//    return m_mesh->GetLocalBounds().Transform(GetTransform().GetWorldMatrix());
//}