#pragma once
#include "MeshRenderer.h"

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

public:
	virtual ~SkinnedMeshRenderer();
	static SkinnedMeshRendererUPtr Create
	(
		SkinnedMeshPtr mesh, 
		MaterialPtr material, 
		Animator* animator
	);
	static const ComponentType s_ComponentType = ComponentType::SkinnedMeshRenderer;
	virtual ComponentType GetComponentType() const override { return s_ComponentType; }
	virtual RenderBounds GetWorldBounds() const override;
	Animator* GetAnimator() const;
	virtual void Render(Program* program) const override;

private:
	SkinnedMeshRenderer();
	bool Init(SkinnedMeshPtr mesh, MaterialPtr material, Animator* animator);
	mutable Animator* m_referenceAnimator{ nullptr };
};