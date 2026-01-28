#pragma once
#include "Resources/Meshes/Mesh.h"
#include "Graphics/Vertex.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(DebugMesh)
class DebugMesh : public Mesh
{
	DEFINE_RESOURCE_TYPE(ResourceType::DebugMesh)

public:
	virtual ~DebugMesh();
	static DebugMeshUPtr Create(uint32 primitiveType);
	virtual void Draw() const override;
	void UpdateGeometry(const std::vector<GizmoVertex>& vertices);

private:
	DebugMesh();
	void Init(uint32 primitiveType);
	void AllocateVertexBuffer(usize vertexCount);

	uint32 m_vertexCount = 0;
	usize m_vertexCapacity = 0;
};