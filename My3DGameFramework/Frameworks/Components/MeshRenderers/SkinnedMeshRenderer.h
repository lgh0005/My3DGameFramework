#pragma once
#include "Components/MeshRenderers/MeshRenderer.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Mesh)
	MGF_CLASS_PTR(SkinnedMesh)
	MGF_CLASS_PTR(Material)

	MGF_CLASS_PTR(SkinnedMeshRenderer)
	class SkinnedMeshRenderer : public MeshRenderer
	{
		MGF_DISABLE_COPY(SkinnedMeshRenderer)
		using Super = MeshRenderer;

	public:
		SkinnedMeshRenderer
		(
			ObjectIDHash id, ObjectIDHash ownerID,
			const MeshPtr& mesh, const MaterialPtr& material
		);
		virtual ~SkinnedMeshRenderer() override;
		SkinnedMeshRenderer(SkinnedMeshRenderer&& other) noexcept;
		SkinnedMeshRenderer& operator=(SkinnedMeshRenderer&& other) noexcept;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		SkinnedMesh* GetSkinnedMesh() const;
		void SetBoneBufferOffset(uint32 offset) { m_boneBufferOffset = offset; }
		uint32 GetBoneBufferOffset() const { return m_boneBufferOffset; }

	private:
		uint32 m_boneBufferOffset { 0 };
	};
}