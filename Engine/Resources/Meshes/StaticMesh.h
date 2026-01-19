#pragma once
#include "Resources/Meshes/Mesh.h"
#include "Graphics/Vertex.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(StaticMesh)
class StaticMesh : public Mesh
{
public:
    static const ResourceType s_ResourceType = ResourceType::StaticMesh;
    virtual ResourceType GetResourceType() const override { return ResourceType::StaticMesh; }
    static StaticMeshUPtr Create
    (
        const std::vector<StaticVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType = GL_TRIANGLES
    );
    virtual ~StaticMesh() override;
    void ComputeTangents
    (
        std::vector<StaticVertex>& vertices,
        const std::vector<uint32>& indices
    );
    virtual void Draw() const override;

private:
    StaticMesh();
    void Init
    (
        const std::vector<StaticVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType
    );
};