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

CLASS_PTR(StaticMesh)
class StaticMesh : public Mesh
{
    DEFINE_RESOURCE_TYPE(ResourceType::StaticMesh, StaticMeshDesc)

public:
    static StaticMeshPtr Create
    (
        const std::vector<StaticVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType = GL_TRIANGLES
    );
    static StaticMeshPtr Load(const StaticMeshDesc& desc);
    virtual StaticMeshDesc& GetDesc() override { return m_desc; }
    virtual const ResourceDesc& GetDesc() const override { return m_desc; }
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
    StaticMeshDesc m_desc;
};