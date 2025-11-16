#include "EnginePch.h"
#include "VertexLayout.h"

VertexLayoutUPtr VertexLayout::Create()
{
    auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
    vertexLayout->Init();
    return move(vertexLayout);
}

VertexLayout::~VertexLayout()
{
    if (m_vertexArrayObject)
    {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
    }
}

void VertexLayout::Bind() const
{
    glBindVertexArray(m_vertexArrayObject);
}

void VertexLayout::SetAttrib(uint32 attribIndex, int count, uint32 type,
            bool normalized, usize stride, uint64 offset) const
{
    glEnableVertexAttribArray(attribIndex);
    glVertexAttribPointer(attribIndex, count,
        type, normalized, stride, (const void*)offset);
}

void VertexLayout::SetAttribI(uint32 attribIndex, int count, uint32 type,
           usize stride, uint64 offset) const
{
    glEnableVertexAttribArray(attribIndex);
    glVertexAttribIPointer(attribIndex, count,
        type, stride, (const void*)offset);
}

void VertexLayout::SetAttribDivisor(uint32 attribIndex, uint32 divisor) const
{
    glVertexAttribDivisor(attribIndex, divisor);
}

void VertexLayout::DisableAttrib(uint32 attribIndex) const
{
    glDisableVertexAttribArray(attribIndex);
}

void VertexLayout::Init()
{
    glGenVertexArrays(1, &m_vertexArrayObject);
    Bind();
}