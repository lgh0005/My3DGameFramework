#include "CorePch.h"
#include "GLTextureBuffer.h"

namespace MGF3D
{
	GLTextureBuffer::GLTextureBuffer() = default;
	GLTextureBuffer::~GLTextureBuffer()
	{
		if (m_texture)
		{
			glDeleteTextures(1, &m_texture);
			m_texture = 0;
		}
	}

	GLTextureBufferUPtr GLTextureBuffer::Create(uint32 format, const void* data, usize byteSize)
	{
		auto buffer = GLTextureBufferUPtr(new GLTextureBuffer());
		if (!buffer->Init(format, data, byteSize)) return nullptr;
		return buffer;
	}

	bool GLTextureBuffer::Init(uint32 format, const void* data, usize byteSize)
	{
		if (byteSize == 0) return false;

		m_byteSize = byteSize;
		m_format = format;

		// 1. 실제 데이터를 담을 버퍼 생성 (DSA)
		glCreateBuffers(1, &m_handle);
		glNamedBufferData(m_handle, byteSize, data, GL_STATIC_DRAW);

		// 2. 버퍼를 텍스처로 바라보게 할 텍스처 객체 생성 (DSA)
		glCreateTextures(GL_TEXTURE_BUFFER, 1, &m_texture);

		// 3. 텍스처 핸들과 버퍼 핸들을 연결 (DSA)
		glTextureBuffer(m_texture, format, m_handle);
		if (m_handle == 0 || m_texture == 0)
		{
			MGF_LOG_ERROR("GLTextureBuffer: Failed to create TBO.");
			return false;
		}

		return false;
	}

	void GLTextureBuffer::Bind(uint32 unit) const
	{
		if (m_texture == 0) return;
		glBindTextureUnit(unit, m_texture);
	}

	void GLTextureBuffer::Unbind(uint32 unit) const
	{
		glBindTextureUnit(unit, 0);
	}

	void GLTextureBuffer::UpdateData(const void* data, usize offset, usize size)
	{
		if (m_handle == 0 || data == nullptr) return;
		if (offset + size > m_byteSize) return;
		glNamedBufferSubData(m_handle, offset, size, data);
	}

	uint32 GLTextureBuffer::GetTexture() const
	{
		return m_texture;
	}
}