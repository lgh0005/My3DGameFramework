#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer()
{
	glGenBuffers(1, &_ibo);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &_ibo);
}

void IndexBuffer::Create(const vector<uint32>& indices)
{
	_count = static_cast<GLuint>(indices.size());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _count * sizeof(uint32), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
