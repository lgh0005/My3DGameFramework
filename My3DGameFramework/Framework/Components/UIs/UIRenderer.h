#pragma once
#include "Object/Component.h"
#include "Graphics/Geometry/RenderBounds.h"

namespace MGF3D
{
	MGF_CLASS_PTR(UIMesh)
	MGF_CLASS_PTR(Material)
	MGF_CLASS_PTR(Program)

	MGF_CLASS_PTR(UIRenderer)
	class UIRenderer : public Component
	{
	public:
		virtual ~UIRenderer();
		virtual RenderBounds GetWorldBounds() const;
		virtual void Render(Program* program) = 0;

		void SetMesh(UIMeshPtr mesh) { m_uiMesh = mesh; }
		UIMeshPtr GetMesh() const { return m_uiMesh; }

		void SetMaterial(MaterialPtr material) { m_material = material; }
		MaterialPtr GetMaterial() const { return m_material; }

		void SetColor(const glm::vec4& color) { m_color = color; }
		glm::vec4 GetColor() const { return m_color; }
		void SetAlpha(float alpha) { m_color.a = alpha; }

		void SetSortingOrder(int32 order) { m_elementSortingOrder = order; }
		int32 GetSortingOrder() const { return m_elementSortingOrder; }

	protected:
		UIRenderer();
		UIMeshPtr m_uiMesh;
		MaterialPtr m_material;
		glm::vec4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
		int32 m_elementSortingOrder{ 0 };
	};
}
