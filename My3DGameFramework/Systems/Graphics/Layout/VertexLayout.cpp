#include "CorePch.h"
#include "VertexLayout.h"
#include "Managers/MemoryManager.h"

namespace MGF3D
{
    void* VertexLayout::operator new(usize size)
    {
        return MGF_MEMORY.Allocate(size);
    }
    void VertexLayout::operator delete(void* ptr, usize size)
    {
        MGF_MEMORY.Deallocate(ptr, size);
    }

    VertexLayout::VertexLayout() = default;
    VertexLayout::~VertexLayout()
    {
        if (m_vertexArrayObject) glDeleteVertexArrays(1, &m_vertexArrayObject);
    }

    VertexLayoutUPtr VertexLayout::Create()
    {
        auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
        if (!vertexLayout->Init()) return nullptr;
        return move(vertexLayout);
    }

    bool VertexLayout::Init()
    {
        glCreateVertexArrays(1, &m_vertexArrayObject);
        return m_vertexArrayObject != 0;
    }

    void VertexLayout::Bind() const
    {
        glBindVertexArray(m_vertexArrayObject);
    }

    void VertexLayout::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexLayout::SetAttrib
    (
        uint32 vbo,
        uint32 attribIndex, int32 count, uint32 type, bool normalized, usize stride, 
        uint64 relativeOffset, uint64 bufferOffset
    )   const
    {
        glEnableVertexArrayAttrib(m_vertexArrayObject, attribIndex);
        glVertexArrayAttribFormat(m_vertexArrayObject, attribIndex, count, type, normalized, relativeOffset);
        glVertexArrayAttribBinding(m_vertexArrayObject, attribIndex, attribIndex);
        glVertexArrayVertexBuffer(m_vertexArrayObject, attribIndex, vbo, static_cast<GLintptr>(bufferOffset), stride);
    }

    void VertexLayout::SetAttribI
    (
        uint32 vbo,
        uint32 attribIndex, int32 count, uint32 type, usize stride, 
        uint64 relativeOffset, uint64 bufferOffset
    )   const
    {
        glEnableVertexArrayAttrib(m_vertexArrayObject, attribIndex);
        glVertexArrayAttribIFormat(m_vertexArrayObject, attribIndex, count, type, relativeOffset);
        glVertexArrayAttribBinding(m_vertexArrayObject, attribIndex, attribIndex);
        glVertexArrayVertexBuffer(m_vertexArrayObject, attribIndex, vbo, static_cast<GLintptr>(bufferOffset), stride);
    }

    void VertexLayout::SetAttribDivisor(uint32 bindingIndex, uint32 divisor) const
    {
        glVertexArrayBindingDivisor(m_vertexArrayObject, bindingIndex, divisor);
    }

    void VertexLayout::DisableAttrib(uint32 attribIndex) const
    {
        glDisableVertexArrayAttrib(m_vertexArrayObject, attribIndex);
    }

    void VertexLayout::SetIndexBuffer(uint32 ibo) const
    {
        glVertexArrayElementBuffer(m_vertexArrayObject, ibo);
    }
}