#pragma once
#include "Meshes/Mesh.h"
#include "Descriptors/StaticMeshDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(StaticMesh)
	class StaticMesh : public Mesh
	{
		using Super = Mesh;
		MGF_DECLARE_CUSTOM_ALLOCATORS;

	public:
		static StaticMeshPtr Create(StaticMeshDescriptor&& desc);
		virtual ~StaticMesh() override;

	public:
		virtual bool OnLoad() override;
		virtual bool OnCommit() override;
		virtual void OnRelease() override;

	private:
		StaticMesh(StaticMeshDescriptor& desc);
		bool Init(StaticMeshDescriptor&& desc);

		void ComputeTangents();
		SVector<StaticGeometryData> m_meshData;
	};
}