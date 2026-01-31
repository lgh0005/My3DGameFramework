#pragma once
#include "Object/Component.h"
#include "Graphics/Geometry/RenderBounds.h"

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
	using Super = Component;
	DEFINE_COMPONENT_TYPE(ComponentType::MeshRenderer)

public:
	virtual ~MeshRenderer();
	virtual bool MatchesType(ComponentType type) const override;
	virtual RenderBounds GetWorldBounds() const;
	virtual void Render(Program* program) const = 0;
	virtual void UpdateInstanceProperty() = 0;

	void SetMesh(MeshPtr mesh) { m_mesh = mesh; }
	MeshPtr GetMesh() const { return m_mesh; }
	void SetRenderStage(RenderStage stage) { m_renderStage = stage; }
	RenderStage GetRenderStage() const { return m_renderStage; }
	void SetMaterial(MaterialPtr material) { m_material = material; }
	MaterialPtr GetMaterial() const { return m_material; }

protected:
	MeshRenderer();
	MeshPtr m_mesh;
	MaterialPtr m_material;
	RenderStage m_renderStage	{ RenderStage::Deferred };
};