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

	void SkinnedMeshRenderer::SetDynamicLocalBounds(const RenderBounds& bounds)
	{
		m_dynamicLocalBounds = bounds;
	}

	RenderBounds SkinnedMeshRenderer::GetDynamicLocalBounds() const
	{
		// 1. Animator가 유효한 동적 바운즈를 계산해서 넣어주었다면 그것을 반환
		if (m_dynamicLocalBounds.IsValid()) return m_dynamicLocalBounds;

		// 2. 초기 상태이거나 Animator가 없다면 공유 중인 Mesh 원본의 기본 바운드 반환
		if (m_mesh) return m_mesh->GetLocalBounds();

		// 3. 메쉬조차 없는 최악의 경우
		return RenderBounds::Empty();
	}
}