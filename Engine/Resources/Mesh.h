#pragma once
#include "Resources/Resource.h"
#include "Graphics/Geometry.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(Mesh)
class Mesh : public Resource
{
    using Super = Resource;

public:
    virtual ~Mesh();
    virtual void Draw(const Program* program) const = 0;

    usize GetIndexCount() const { return m_indexCount; }
    BufferPtr GetIndexBuffer() const { return m_indexBuffer; }
    const VertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }
    BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }

    void SetMaterial(MaterialPtr material) { m_material = material; }
    MaterialPtr GetMaterial() const { return m_material; }
    void SetLocalBounds(const RenderBounds& bounds) { m_localBounds = bounds; }
    const RenderBounds& GetLocalBounds() const { return m_localBounds; }

protected:
    Mesh() = default;
    usize  m_indexCount;
    uint32 m_primitiveType;
    VertexLayoutUPtr m_vertexLayout;
    BufferPtr m_vertexBuffer;
    BufferPtr m_indexBuffer;
    MaterialPtr m_material;
    RenderBounds m_localBounds;
};
