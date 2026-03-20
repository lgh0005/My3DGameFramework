#pragma once
#include "Meshes/SubMesh.h"

namespace MGF3D
{
	class SubMeshBuilder
	{
		MGF_DECLARE_UTILITIES(SubMeshBuilder)

	public:
		template<typename TVertex> static SubMesh BuildSubMesh
		(
			const SVector<TVertex>& vertices,
			const SVector<uint32>& indices
		);

	private:
		template<typename TVertex> static void SetupVertexLayout
		(
			Ptr<VertexLayout> layout, 
			const GLVertexBuffer& vbo
		);
	};
}

#include "Utils/SubMeshBuilder.inl"