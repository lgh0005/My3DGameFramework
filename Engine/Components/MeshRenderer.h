#pragma once
#include "Object/Component.h"
#include "Graphics/Geometry.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

enum class RenderStage
{
	Deferred,  // G-Buffer 단계
	Forward    // 후처리 전 단계
};

CLASS_PTR(MeshRenderer)
class MeshRenderer : public Component
{
	DEFINE_COMPONENT_TYPE(ComponentType::MeshRenderer)
	using Super = Component;

public:
	virtual ~MeshRenderer();
	static const ComponentType s_ComponentType = ComponentType::MeshRenderer;
	virtual bool MatchesType(ComponentType type) const override;
	virtual RenderBounds GetWorldBounds() const;
	virtual void Render(Program* program) const = 0;

	void SetMesh(MeshPtr mesh) { m_mesh = mesh; }
	MeshPtr GetMesh() const { return m_mesh; }
	void SetRenderStage(RenderStage stage) { m_renderStage = stage; }
	RenderStage GetRenderStage() const { return m_renderStage; }

	void SetMaterial(MaterialPtr material) { m_material = material; }
	MaterialPtr GetMaterial() const { return m_material; }
	void SetMaterialIndex(uint32 index) { m_materialIndex = index; }
	uint32 GetMaterialIndex() const { return m_materialIndex; }

protected:
	MeshRenderer();
	MeshPtr m_mesh;
	MaterialPtr m_material;
	uint32 m_materialIndex{ 0 };
	RenderStage m_renderStage	{ RenderStage::Deferred };
};