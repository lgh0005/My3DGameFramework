#pragma once
#include "Resources/Mesh.h"
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
public:
	virtual ~DebugMesh();
	static DebugMeshUPtr Create(uint32 primitiveType);
	static const ResourceType s_ResourceType = ResourceType::DebugMesh;
	virtual ResourceType GetResourceType() const override { return ResourceType::DebugMesh; }
	virtual void Draw() const override;
	void UpdateGeometry(const std::vector<GizmoVertex>& vertices);

private:
	DebugMesh();
	void Init(uint32 primitiveType);

	uint32 m_vertexCount = 0;
	usize m_bufferCapacity = 0;
};