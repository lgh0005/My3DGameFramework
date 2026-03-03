#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Meshes/Mesh.h"
#include "Graphics/Layouts/Vertex.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(SkinnedMesh)
class SkinnedMesh : public Mesh
{
    DEFINE_RESOURCE_TYPE(ResourceType::SkinnedMesh, SkinnedMeshDesc)

public:
    static SkinnedMeshPtr Create
    (
        const std::vector<SkinnedVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType = GL_TRIANGLES
    );
    static SkinnedMeshPtr Load(const SkinnedMeshDesc& desc);
    virtual ~SkinnedMesh() override;
    virtual SkinnedMeshDesc& GetDesc() override { return m_desc; }
    virtual const ResourceDesc& GetDesc() const override { return m_desc; }
    virtual void Draw() const override;

private:
    SkinnedMesh();
    void Init
    (
        const std::vector<SkinnedVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType
    );
    SkinnedMeshDesc m_desc;
};
