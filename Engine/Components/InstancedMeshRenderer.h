#pragma once
#include "MeshRenderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(InstancedMesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(InstancedMeshRenderer)
class InstancedMeshRenderer : public MeshRenderer
{
	using Super = MeshRenderer;
	DEFINE_COMPONENT_TYPE(ComponentType::InstancedMeshRenderer)

public:
	virtual ~InstancedMeshRenderer();
	static InstancedMeshRendererUPtr Create(InstancedMeshPtr mesh, MaterialPtr material);
	virtual RenderBounds GetWorldBounds() const override;
	virtual void Render(Program* program) const;

private:
	InstancedMeshRenderer();
	bool Init(InstancedMeshPtr mesh, MaterialPtr material);
	InstancedMesh* m_instancedMeshCache	{ nullptr };
};