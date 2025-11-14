#pragma once
#include "Graphics/Vertex.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(Mesh)
class Mesh
{
public:
    ~Mesh(); // TODO : 나중에 소멸자 어떻게 수정할 수 있는 지 고려 필요
    static MeshUPtr Create
    (
        const std::vector<Vertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType
    );

    const VertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }
    BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
    BufferPtr GetIndexBuffer() const { return m_indexBuffer; }
    void SetMaterial(MaterialPtr material) { m_material = material; }
    MaterialPtr GetMaterial() const { return m_material; }
    void Draw(const Program* program) const;

/*==============//
//  mesh utils  //
//==============*/
public:
    static MeshUPtr CreateBox();
    static MeshUPtr CreatePlane();

private:
    Mesh() = default;
    void Init
    (
        const std::vector<Vertex>& vertices,
        const std::vector<uint32>& indices,
        uint32 primitiveType
    );

    uint32 m_primitiveType  { GL_TRIANGLES };
    VertexLayoutUPtr m_vertexLayout;
    BufferPtr m_vertexBuffer;
    BufferPtr m_indexBuffer;
    MaterialPtr m_material;
};
