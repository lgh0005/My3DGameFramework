#include "GraphicsPch.h"
#include "StaticMesh.h"
#include "Utils/SubMeshBuilder.h"

namespace MGF3D
{
	MGF_IMPLEMENT_CUSTOM_ALLOCATORS(StaticMesh)

	StaticMesh::StaticMesh(const IResourceDescriptor& desc) : Super(desc) { }
	StaticMesh::~StaticMesh() = default;

	StaticMeshPtr StaticMesh::Create
	(
		const IResourceDescriptor& desc,
		SVector<StaticVertex>&& vertices,
		SVector<uint32>&& indices
	)
	{
		StaticMeshPtr mesh = StaticMeshPtr(new StaticMesh(desc));
		if (!mesh->Init(std::move(vertices), std::move(indices))) return nullptr;
		return mesh;
	}

	bool StaticMesh::Init
	(
		const IResourceDescriptor& desc,
		SVector<StaticVertex>&& vertices,
		SVector<uint32>&& indices
	)
	{
		if (vertices.Count() == 0 || indices.Count() == 0) return false;
		m_vertices = std::move(vertices);
		m_indices = std::move(indices);
		return true;
	}

	bool StaticMesh::OnLoad()
	{
		if (m_vertices.Count() == 0)
		{
			MGF_LOG_ERROR("[StaticMesh] Vertex or index data is empty. Cannot process mesh.");
			return false;
		}

		ComputeTangents();

		return true;
	}

	bool StaticMesh::OnCommit()
	{
		// 1. 데이터 검증
		if (m_vertices.Count() == 0 || m_indices.Count() == 0)
		{
			MGF_LOG_ERROR("[StaticMesh] No data to commit to VRAM.");
			return false;
		}

		// 2. SubMeshBuilder를 통해 OpenGL VRAM 버퍼 및 레이아웃 생성
		SubMesh subMesh = SubMeshBuilder::BuildSubMesh<StaticVertex>(m_vertices, m_indices);
		if (!subMesh.vertexBuffer || !subMesh.indexBuffer)
		{
			MGF_LOG_ERROR("[StaticMesh] Failed to create VRAM buffers during Commit.");
			return false;
		}

		AddSubMesh(std::move(subMesh));

		return false;
	}

	void StaticMesh::OnRelease()
	{
		m_vertices.Clear();
		m_indices.Clear();
	}

	void StaticMesh::ComputeTangents()
	{
	}
}