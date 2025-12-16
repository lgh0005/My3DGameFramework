#pragma once
#include "Component.h"
#include "Graphics/Geometry.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(StaticMesh)
CLASS_PTR(Material)
#pragma endregion

enum class RenderStage
{
	Deferred,  // G-Buffer 단계
	Forward    // 후처리 전 단계
};

CLASS_PTR(MeshRenderer)
class MeshRenderer : public Component
{
public:
	static MeshRendererUPtr Create(MeshPtr mesh, MaterialPtr material);

	static const ComponentType s_ComponentType = ComponentType::MeshRenderer;
	virtual ComponentType GetComponentType() const override { return s_ComponentType; }

	MeshPtr GetMesh() const { return m_mesh; }
	void SetMesh(MeshPtr mesh) { m_mesh = mesh; }
	MaterialPtr GetMaterial() const { return m_material; }
	void SetMaterial(MaterialPtr material) { m_material = material; }
	RenderStage GetRenderStage() const { return m_renderStage; }
	void SetRenderStage(RenderStage stage) { m_renderStage = stage; }

	RenderBounds GetWorldBounds() const;

private:
	MeshRenderer() = default;
	bool Init(MeshPtr mesh, MaterialPtr material);

	MeshPtr m_mesh;
	MaterialPtr m_material;
	RenderStage m_renderStage	{ RenderStage::Deferred };
};