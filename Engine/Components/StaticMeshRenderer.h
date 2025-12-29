#pragma once
#include "MeshRenderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(StaticMesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(StaticMeshRenderer)
class StaticMeshRenderer : public MeshRenderer
{
	using Super = MeshRenderer;

public:
	virtual ~StaticMeshRenderer();
	static StaticMeshRendererUPtr Create(StaticMeshPtr mesh, MaterialPtr material);
	static const ComponentType s_ComponentType = ComponentType::StaticMeshRenderer;
	virtual ComponentType GetComponentType() const override { return s_ComponentType; }
	virtual RenderBounds GetWorldBounds() const override;
	virtual void Render(Program* program) const;

private:
	StaticMeshRenderer();
	bool Init(StaticMeshPtr mesh, MaterialPtr material);
};