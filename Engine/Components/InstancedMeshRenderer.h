#pragma once
#include "MeshRenderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(InstancedMesh)
CLASS_PTR(Material)
#pragma endregion

CLASS_PTR(InstancedMeshRenderer)
class InstancedMeshRenderer : public MeshRenderer
{
	using Super = MeshRenderer;

public:
	virtual ~InstancedMeshRenderer();
	static InstancedMeshRendererUPtr Create(InstancedMeshPtr mesh, MaterialPtr material);
	static const ComponentType s_ComponentType = ComponentType::InstancedMeshRenderer;
	virtual ComponentType GetComponentType() const override { return s_ComponentType; }
	virtual RenderBounds GetWorldBounds() const override;

private:
	InstancedMeshRenderer();
	bool Init(InstancedMeshPtr mesh, MaterialPtr material);
};