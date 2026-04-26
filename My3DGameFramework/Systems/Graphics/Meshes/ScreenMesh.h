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
		virtual bool OnSyncCreate() override;

	/*=====================//
	//   ScreenMesh Type   //
	//=====================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	private:
		ScreenMesh();

		Vector<ScreenVertex> m_vertices;
		Vector<uint32> m_indices;
	};
}