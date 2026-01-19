#pragma once
#include "Resources/Meshes/Mesh.h"
#include "Graphics/Vertex.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(SkinnedMesh)
class SkinnedMesh : public Mesh
{
public:
    static const ResourceType s_ResourceType = ResourceType::SkinnedMesh;
    virtual ResourceType GetResourceType() const override { return ResourceType::SkinnedMesh; }
    static SkinnedMeshUPtr Create
    (
        const std::vector<SkinnedVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType = GL_TRIANGLES
    );
    virtual ~SkinnedMesh() override;
    virtual void Draw() const override;

private:
    SkinnedMesh();
    void Init
    (
        const std::vector<SkinnedVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType
    );
};
