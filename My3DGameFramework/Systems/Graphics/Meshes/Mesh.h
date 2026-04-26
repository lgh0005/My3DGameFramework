#pragma once
#include "Sources/Resource.h"
#include "Geometry/RenderBounds.h"

namespace MGF3D
{
    MGF_CLASS_PTR(GLVertexLayout)
    MGF_CLASS_PTR(GLVertexBuffer)
    MGF_CLASS_PTR(GLIndexBuffer)

    MGF_CLASS_PTR(Mesh)
    class Mesh : public Resource
    {
    public:
        virtual ~Mesh();
        virtual void Bind() const;
        virtual void Unbind() const;

    /*===============//
    //   Mesh Type   //
    //===============*/
    public:
        static int16 s_typeIndex;
        virtual const MGFType* GetType() const override;

    public:
        GLIndexBufferPtr GetIndexBuffer() const { return m_indexBuffer; }
        GLVertexBufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
        const GLVertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }

    public:
        usize GetIndexCount() const { return m_indexCount; }
        uint32 GetPrimitiveType() const { return m_primitiveType; }
       
    public:
        void SetLocalBounds(const RenderBounds& bounds) { m_localBounds = bounds; }
        const RenderBounds& GetLocalBounds() const { return m_localBounds; }

    protected:
        Mesh();
        usize  m_indexCount        { 0 };
        uint32 m_primitiveType     { GL_TRIANGLES };

        GLVertexLayoutUPtr m_vertexLayout;
        GLVertexBufferPtr m_vertexBuffer;
        GLIndexBufferPtr m_indexBuffer;

        RenderBounds m_localBounds { RenderBounds() };
    };
}