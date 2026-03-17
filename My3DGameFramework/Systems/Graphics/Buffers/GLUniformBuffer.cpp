#include "GraphicsPch.h"
#include "GLUniformBuffer.h"

namespace MGF3D
{
	GLUniformBuffer::GLUniformBuffer() = default;
	GLUniformBuffer::~GLUniformBuffer() = default;

	GLUniformBufferUPtr GLUniformBuffer::Create(usize byteSize, uint32 flags)
	{
		auto buffer = GLUniformBufferUPtr(new GLUniformBuffer());
		if (!buffer->Init(byteSize, flags)) return nullptr;
		return buffer;
	}

	bool GLUniformBuffer::Init(usize byteSize, uint32 flags)
	{
		glNamedBufferStorage(m_buffer, byteSize, nullptr, flags);
		return true;
	}

	void GLUniformBuffer::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
	}

	void GLUniformBuffer::BindBase(uint32 slot) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, m_buffer);
	}

	void GLUniformBuffer::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}