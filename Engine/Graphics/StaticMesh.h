#pragma once
#include "Graphics/Mesh.h"
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
    static StaticMeshUPtr Create
    (
        const std::vector<StaticVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType = GL_TRIANGLES
    );
    virtual ~StaticMesh() override;
    virtual MeshType GetMeshType() const override { return MeshType::Static; }
    void ComputeTangents
    (
        std::vector<StaticVertex>& vertices,
        const std::vector<uint32>& indices
    );
    virtual void Draw(const Program* program) const override;

private:
    StaticMesh() = default;
    void Init
    (
        const std::vector<StaticVertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType
    );

/*==============//
//  mesh utils  //
//==============*/
public:
    static StaticMeshUPtr CreateBox();
    static StaticMeshUPtr CreatePlane();
    static StaticMeshUPtr CreateNDCQuad();
};