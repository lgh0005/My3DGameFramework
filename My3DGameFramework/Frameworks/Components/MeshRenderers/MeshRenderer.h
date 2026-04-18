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
