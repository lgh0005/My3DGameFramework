#include "pch.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &_vao);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &_vao);
}

void VertexArray::AddVertexBuffer(const shared_ptr<VertexBuffer>& vbo, const vector<GLuint>& attribSizes)
{
	glBindVertexArray(_vao);
    vbo->Bind();

    uint32 offset = 0;
    uint32 index = 0;
    for (auto size : attribSizes)
    {
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, vbo->GetStride(), reinterpret_cast<void*>(offset));
        glEnableVertexAttribArray(index);
        offset += size * sizeof(float);
        index++;
    }

    vbo->Unbind();
	glBindVertexArray(0);
}

void VertexArray::SetIndexBuffer(const shared_ptr<IndexBuffer>& ibo)
{
    glBindVertexArray(_vao);
    ibo->Bind();
    glBindVertexArray(0);
}