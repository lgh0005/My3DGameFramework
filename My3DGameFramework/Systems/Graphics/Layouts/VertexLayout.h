#pragma once
#include "Pointer/PoolAlloc.h"
#include "Buffers/GLVertexBuffer.h"
#include "Buffers/GLIndexBuffer.h"

namespace MGF3D
{
    MGF_CLASS_PTR(VertexLayout)
    class VertexLayout : public PoolAlloc
    {
    public:
        ~VertexLayout();
        static VertexLayoutUPtr Create();

    public:
        uint32 Get() const { return m_vertexArrayObject; }
        void Bind() const;
        void Unbind() const;

    public:
        void SetAttrib
        (
            const GLVertexBuffer& vertexBuffer,
            uint32 attribIndex, int32 count, uint32 type, bool normalized, usize stride, 
            uint64 relativeOffset, uint64 bufferOffset = 0
        ) const;

        void SetAttribI
        (
            const GLVertexBuffer& vertexBuffer,
            uint32 attribIndex, int32 count, uint32 type, usize stride, 
            uint64 relativeOffset, uint64 bufferOffset = 0
        ) const;

        void SetAttribDivisor(uint32 bindingIndex, uint32 divisor) const;
        void DisableAttrib(uint32 attribIndex) const;

    public:
        void SetIndexBuffer(const GLIndexBuffer& indexBuffer) const;

    private:
        VertexLayout();
        bool Init();
        uint32 m_vertexArrayObject{ 0 };
    };
}