#include "GraphicsPch.h"
#include "GLIndexBuffer.h"

namespace MGF3D
{
	GLIndexBuffer::GLIndexBuffer() = default;
	GLIndexBuffer::~GLIndexBuffer() = default;

	GLIndexBufferUPtr GLIndexBuffer::Create
	(
		const void* data, 
		usize byteSize,
		uint32 flags
	)
	{
		auto indexBuffer = GLIndexBufferUPtr(new GLIndexBuffer());
		if (!indexBuffer->Init(data, byteSize, flags)) return nullptr;
		return std::move(indexBuffer);
	}

	bool GLIndexBuffer::Init(const void* data, usize byteSize, uint32 flags)
	{
		glNamedBufferStorage(m_buffer, byteSize, data, flags);
		return true;
	}

	void GLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
	}

	void GLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}