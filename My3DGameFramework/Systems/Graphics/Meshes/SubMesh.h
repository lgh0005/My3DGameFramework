#pragma once
#include "Spatial/BoundingBox.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLVertexBuffer)
	MGF_CLASS_PTR(GLIndexBuffer)
	MGF_CLASS_PTR(VertexLayout)

	struct SubMesh
	{
		SubMesh();
		~SubMesh();
		SubMesh(SubMesh&& other) noexcept;
		SubMesh& operator=(SubMesh&& other) noexcept;

		GLVertexBufferUPtr vertexBuffer;
		GLIndexBufferUPtr  indexBuffer;
		VertexLayoutUPtr   vertexLayout;

		uint32 indexCount    { 0 };
		uint32 vertexCount   { 0 };
		uint32 materialIndex { 0 };

		BoundingBox bounds;
	};
}