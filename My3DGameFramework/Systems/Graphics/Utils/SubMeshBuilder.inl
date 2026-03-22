#pragma once
#include "Spatial/AABB.h"
#include "SubMeshBuilder.h"
#include "Buffers/GLVertexBuffer.h"
#include "Buffers/GLIndexBuffer.h"
#include "Layouts/VertexLayout.h"

namespace MGF3D
{
	template<typename TVertex>
	inline SubMesh SubMeshBuilder::BuildSubMesh
	(
		const SVector<TVertex>& vertices,
		const SVector<uint32>& indices
	)
	{
		SubMesh subMesh;
		subMesh.vertexCount = static_cast<uint32>(vertices.Count());
		subMesh.indexCount = static_cast<uint32>(indices.Count());

		// 1. AABB 바운딩 박스 계산
		AABB localBounds(vec3(FloatMax), vec3(FloatLowest));
		for (const auto& v : vertices)
			localBounds.Encapsulate(v.position);
		subMesh.bounds = BoundingBox(localBounds.min, localBounds.max);

		// 2. VRAM 할당
		usize vbSize = vertices.Count() * sizeof(TVertex);
		usize ibSize = indices.Count() * sizeof(uint32);
		subMesh.vertexBuffer = GLVertexBuffer::Create(vertices.Data(), vbSize, VRAMAllocation::PoolType::Static);
		subMesh.indexBuffer  = GLIndexBuffer::Create(indices.Data(), ibSize, VRAMAllocation::PoolType::Static);
		if (!subMesh.vertexBuffer || !subMesh.indexBuffer)
		{
			MGF_LOG_ERROR("SubMeshBuilder: Failed to create VRAM buffers.");
			return subMesh;
		}

		// 3. 타입에 맞는 VertexLayout 구성
		subMesh.vertexLayout = VertexLayout::Create();
		if (subMesh.vertexLayout)
		{
			SetupVertexLayout<TVertex>(subMesh.vertexLayout.Get(), *subMesh.vertexBuffer);
			subMesh.vertexLayout->SetIndexBuffer(*subMesh.indexBuffer);
		}

		return subMesh;
	}
}