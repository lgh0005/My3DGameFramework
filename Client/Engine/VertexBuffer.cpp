#include "pch.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &_vbo);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &_vbo);
}
