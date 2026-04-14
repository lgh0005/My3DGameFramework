#pragma once
#include "Entities/Component.h"
#include "Geometry/RenderBounds.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Mesh)
	MGF_CLASS_PTR(Material)

	MGF_CLASS_PTR(MeshRenderer)
	class MeshRenderer : public Component
	{
		MGF_DISABLE_COPY(MeshRenderer)
		using Super = Component;

	public:
		MeshRenderer(ObjectIDHash id, ObjectIDHash ownerID);
		virtual ~MeshRenderer() override;
		MeshRenderer(MeshRenderer&& other) noexcept;
		MeshRenderer& operator=(MeshRenderer&& other) noexcept;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	/*======================================//
	//     default MeshRenderer methods     //
	//======================================*/
	public:
		void SetMesh(MeshPtr mesh) { m_mesh = mesh; }
		MeshPtr GetMesh() const { return m_mesh; }
		void SetMaterial(MaterialPtr material) { m_material = material; }
		MaterialPtr GetMaterial() const { return m_material; }

	private:
		MeshPtr m_mesh			{ nullptr };
		MaterialPtr m_material	{ nullptr };

	};
}


//#pragma region FORWARD_DECLARATION
//CLASS_PTR(Mesh)
//CLASS_PTR(Material)
//CLASS_PTR(Program)
//#pragma endregion
//
//enum class RenderStage
//{
//	Deferred,  // G-Buffer 단계
//	Forward    // 후처리 전 단계
//};
//
//CLASS_PTR(MeshRenderer)
//class MeshRenderer : public Component
//{
//	using Super = Component;
//	DEFINE_COMPONENT_TYPE(ComponentType::MeshRenderer)
//
//public:
//	virtual ~MeshRenderer();
//	virtual bool MatchesType(ComponentType type) const override;
//	virtual RenderBounds GetWorldBounds() const;
//	virtual void Render(Program* program) const = 0;
//
//	void SetMesh(MeshPtr mesh) { m_mesh = mesh; }
//	MeshPtr GetMesh() const { return m_mesh; }
//	void SetRenderStage(RenderStage stage) { m_renderStage = stage; }
//	RenderStage GetRenderStage() const { return m_renderStage; }
//	void SetMaterial(MaterialPtr material) { m_material = material; }
//	MaterialPtr GetMaterial() const { return m_material; }
//	void SetColor(const glm::vec4& color) { m_color = color; }
//	const glm::vec4& GetColor() const { return m_color; }
//
//protected:
//	MeshRenderer();
//	MeshPtr m_mesh;
//	MaterialPtr m_material;
//	glm::vec4 m_color			{ 1.0f };
//	RenderStage m_renderStage	{ RenderStage::Deferred };
//};