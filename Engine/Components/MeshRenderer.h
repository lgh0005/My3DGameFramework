#pragma once
#include "Components/Bases/Component.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(StaticMesh)
CLASS_PTR(Material)
#pragma endregion

CLASS_PTR(MeshRenderer)
class MeshRenderer : public Component
{
public:
	static MeshRendererUPtr Create(MeshPtr mesh, MaterialPtr material);

	static const ComponentType s_ComponentType = ComponentType::MeshRenderer;
	virtual ComponentType GetType() const override { return s_ComponentType; }

	MeshPtr GetMesh() const { return m_mesh; }
	MaterialPtr GetMaterial() const { return m_material; }
	void SetMesh(MeshPtr mesh) { m_mesh = mesh; }
	void SetMaterial(MaterialPtr material) { m_material = material; }

private:
	MeshRenderer() = default;

	bool Init(MeshPtr mesh, MaterialPtr material);

	MeshPtr m_mesh;
	MaterialPtr m_material;
};