#include "GraphicsPch.h"
#include "SubMesh.h"
#include "Buffers/GLVertexBuffer.h"
#include "Buffers/GLIndexBuffer.h"
#include "Layouts/VertexLayout.h"

namespace MGF3D
{
	SubMesh::SubMesh() = default;
	SubMesh::~SubMesh() = default;
	SubMesh::SubMesh(SubMesh&& other) noexcept = default;
	SubMesh& SubMesh::operator=(SubMesh&& other) noexcept = default;
}