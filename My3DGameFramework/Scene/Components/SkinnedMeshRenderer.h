#pragma once
#include "MeshRenderer.h"
#include "Instancing/InstanceProperty.h"
#include "StaticMeshRenderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(SkinnedMesh)
CLASS_PTR(Material)
CLASS_PTR(Animator)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(SkinnedMeshRenderer)
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