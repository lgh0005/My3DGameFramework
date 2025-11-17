#pragma once
#include "Graphics/Mesh.h"
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
    static SkinnedMeshUPtr Create
    (
        const std::vector<SkinnedVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType = GL_TRIANGLES
    );
    virtual ~SkinnedMesh() override;
    virtual MeshType GetMeshType() const override { return MeshType::Skinned; }
    virtual void Draw(const Program* program) const override;

private:
    SkinnedMesh() = default;
    void Init
    (
        const std::vector<SkinnedVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType
    );
};
