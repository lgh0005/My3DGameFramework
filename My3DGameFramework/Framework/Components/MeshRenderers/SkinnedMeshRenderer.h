#pragma once
#include "MeshRenderer.h"
#include "Instancing/InstanceProperty.h"
#include "StaticMeshRenderer.h"

namespace MGF3D
{
	MGF_CLASS_PTR(SkinnedMesh)
	MGF_CLASS_PTR(Material)
	MGF_CLASS_PTR(Animator)
	MGF_CLASS_PTR(Program)

	MGF_CLASS_PTR(SkinnedMeshRenderer)
	class SkinnedMeshRenderer : public MeshRenderer
	{
		using Super = MeshRenderer;
		DEFINE_COMPONENT_TYPE(ComponentType::SkinnedMeshRenderer)

	public:
		virtual ~SkinnedMeshRenderer();
		static SkinnedMeshRendererUPtr Create
		(
			SkinnedMeshPtr mesh,
			MaterialPtr material,
			Animator* animator
		);
		virtual RenderBounds GetWorldBounds() const override;
		Animator* GetAnimator() const;
		virtual void Render(Program* program) const override;

	private:
		SkinnedMeshRenderer();
		bool Init(SkinnedMeshPtr mesh, MaterialPtr material, Animator* animator);
		mutable Animator* m_referenceAnimator{ nullptr };
	};
}
