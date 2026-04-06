#pragma once
#include "MeshRenderer.h"
#include "Instancing/InstanceProperty.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(StaticMesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(StaticMeshRenderer)
class StaticMeshRenderer : public MeshRenderer
{
	using Super = MeshRenderer;
	DEFINE_COMPONENT_TYPE(ComponentType::StaticMeshRenderer)

public:
	virtual ~StaticMeshRenderer();
	static StaticMeshRendererUPtr Create(StaticMeshPtr mesh, MaterialPtr material);
	virtual RenderBounds GetWorldBounds() const override { return Super::GetWorldBounds(); }
	virtual void Render(Program* program) const;

private:
	StaticMeshRenderer();
	bool Init(StaticMeshPtr mesh, MaterialPtr material);
};