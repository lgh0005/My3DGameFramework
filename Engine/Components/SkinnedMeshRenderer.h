#pragma once
#include "MeshRenderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(SkinnedMesh)
CLASS_PTR(Material)
CLASS_PTR(Animator)
#pragma endregion

CLASS_PTR(SkinnedMeshRenderer)
class SkinnedMeshRenderer : public MeshRenderer
{
	using Super = MeshRenderer;

public:
	virtual ~SkinnedMeshRenderer();
	static SkinnedMeshRendererUPtr Create(SkinnedMeshPtr mesh, MaterialPtr material);
	static const ComponentType s_ComponentType = ComponentType::SkinnedMeshRenderer;
	virtual ComponentType GetComponentType() const override { return s_ComponentType; }
	virtual RenderBounds GetWorldBounds() const override;

	Animator* GetAnimator() const;

private:
	SkinnedMeshRenderer();
	bool Init(SkinnedMeshPtr mesh, MaterialPtr material);
	mutable Animator* m_cachedAnimator{ nullptr };
};