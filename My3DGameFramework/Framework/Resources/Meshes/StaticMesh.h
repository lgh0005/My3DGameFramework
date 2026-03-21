#pragma once
#include "Meshes/Mesh.h"
#include "Vertices/StaticVertex.h"

namespace MGF3D
{
	MGF_CLASS_PTR(StaticMesh)
	class StaticMesh : public Mesh
	{
		using Super = Mesh;
		MGF_DECLARE_CUSTOM_ALLOCATORS()

	public:
		static StaticMeshPtr Create
		(
			const IResourceDescriptor& desc,
			SVector<StaticVertex>&& vertices,
			SVector<uint32>&& indices
		);

		virtual ~StaticMesh() override;

	protected:
		virtual bool OnLoad() override;
		virtual bool OnCommit() override;
		virtual void OnRelease() override;

	private:
		StaticMesh(const IResourceDescriptor& desc);
		bool Init
		(
			const IResourceDescriptor& desc,
			SVector<StaticVertex>&& vertices,
			SVector<uint32>&& indices
		);
		void ComputeTangents();

	private:
		SVector<StaticVertex> m_vertices;
		SVector<uint32>       m_indices;
	};
}