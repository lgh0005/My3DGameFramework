#pragma once
#include "MeshRenderer.h"
#include "Instancing/InstanceProperty.h"

namespace MGF3D
{
	MGF_CLASS_PTR(StaticMesh)
	MGF_CLASS_PTR(Material)
	MGF_CLASS_PTR(Program)

	MGF_CLASS_PTR(StaticMeshRenderer)
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
}
