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
		MeshRenderer
		(
			ObjectIDHash id, ObjectIDHash ownerID, 
			MeshPtr mesh, StringHash materialHash
		);
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
		void SetMaterialHash(StringHash materialHash) { m_materialHash = materialHash; }
		StringHash GetMaterialHash() const { return m_materialHash; }

	public:
		void Draw(uint32 count) const;

	private:
		MeshPtr m_mesh			  { nullptr };
		StringHash m_materialHash { 0 };
	};
}
