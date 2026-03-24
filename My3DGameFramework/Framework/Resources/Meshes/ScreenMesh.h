#pragma once
#include "Meshes/Mesh.h"
#include "Descriptors/ScreenMeshDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(ScreenMesh)
	class ScreenMesh : public Mesh
	{
		using Super = Mesh;

	public:
		static ScreenMeshPtr Create(ScreenMeshDescriptor&& desc);
		virtual ~ScreenMesh() override;

	public:
		virtual bool OnLoad() override;
		virtual bool OnCommit() override;
		virtual void OnRelease() override;

	private:
		ScreenMesh(ScreenMeshDescriptor& desc);
		bool Init(ScreenMeshDescriptor&& desc);

		SVector<ScreenGeometryData> m_meshData;
	};
}