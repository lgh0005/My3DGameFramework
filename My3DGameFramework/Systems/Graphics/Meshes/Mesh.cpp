#include "GraphicsPch.h"
#include "Mesh.h"

namespace MGF3D
{
	Mesh::Mesh() = default;
	Mesh::~Mesh() = default;

	void Mesh::AddSubMesh(SubMesh&& subMesh)
	{
		m_subMeshes.PushBack(std::move(subMesh));
	}

	const SVector<SubMesh>& Mesh::GetSubMeshes() const
	{
		return m_subMeshes;
	}

	usize Mesh::GetSubMeshCount() const
	{
		return m_subMeshes.Count();
	}
}