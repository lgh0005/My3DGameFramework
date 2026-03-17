#include "GraphicsPch.h"
#include "GLVertexBuffer.h"

namespace MGF3D
{
	GLVertexBuffer::GLVertexBuffer() = default;
	GLVertexBuffer::~GLVertexBuffer() = default;

	GLVertexBufferUPtr GLVertexBuffer::Create(const void* data, usize byteSize, uint32 flags)
	{
		auto buffer = GLVertexBufferUPtr(new GLVertexBuffer());
		if (!buffer->Init(data, byteSize, flags)) return nullptr;
		return buffer;
	}

	bool GLVertexBuffer::Init(const void* data, usize byteSize, uint32 flags)
	{
		glNamedBufferStorage(m_buffer, byteSize, data, flags);
		return true;
	}

	void GLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	}

	void GLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}