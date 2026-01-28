#pragma once
#include "Resources/Resource.h"
#include "Graphics/Geometry/Geometry.h"

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
    DEFINE_RESOURCE_TYPE(ResourceType::ScreenMesh)

public:
    virtual ~Mesh();
    virtual void Draw() const = 0;

    usize GetIndexCount() const { return m_indexCount; }
    uint32 GetPrimitiveType() const { return m_primitiveType; }
    BufferPtr GetIndexBuffer() const { return m_indexBuffer; }
    BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
    const VertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }
    void SetLocalBounds(const RenderBounds& bounds) { m_localBounds = bounds; }
    const RenderBounds& GetLocalBounds() const { return m_localBounds; }

protected:
    Mesh();
    usize  m_indexCount{ 0 };
    uint32 m_primitiveType{ 0 };
    VertexLayoutUPtr m_vertexLayout;
    BufferPtr m_vertexBuffer;
    BufferPtr m_indexBuffer;
    RenderBounds m_localBounds  { RenderBounds::Empty() };
};
