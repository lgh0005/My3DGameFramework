#pragma once
#include "Meshes/Mesh.h"

namespace MGF3D
{
	MGF_CLASS_PTR(ScreenMesh)
	class ScreenMesh : public Mesh
	{
		using Super = Mesh;

	public:
		virtual ~ScreenMesh() override;
		static ScreenMeshPtr Create();
		virtual void Draw(uint32 count = 1) const override;

	private:
		ScreenMesh();
		void Init();
	};
}