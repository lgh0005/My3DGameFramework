#pragma once
#include "Component.h"
#include "Graphics/Geometry.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
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
	virtual RenderBounds GetWorldBounds() const;

	void SetMesh(MeshPtr mesh) { m_mesh = mesh; }
	MeshPtr GetMesh() const { return m_mesh; }
	void SetMaterial(MaterialPtr material) { m_material = material; }
	MaterialPtr GetMaterial() const { return m_material; }
	void SetRenderStage(RenderStage stage) { m_renderStage = stage; }
	RenderStage GetRenderStage() const { return m_renderStage; }

protected:
	MeshRenderer() = default;
	MeshPtr m_mesh;
	MaterialPtr m_material;
	RenderStage m_renderStage	{ RenderStage::Deferred };
};