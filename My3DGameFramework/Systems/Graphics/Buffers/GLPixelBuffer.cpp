#include "GraphicsPch.h"
#include "GLPixelBuffer.h"

namespace MGF3D
{
	GLPixelBuffer::GLPixelBuffer() = default;
	GLPixelBuffer::~GLPixelBuffer() = default;

	GLPixelBufferUPtr GLPixelBuffer::Create(const void* data, usize byteSize)
	{
		auto buffer = GLPixelBufferUPtr(new GLPixelBuffer());
		if (!buffer->Init(data, byteSize)) return nullptr;
		return buffer;
	}

	bool GLPixelBuffer::Init(const void* data, usize byteSize)
	{
		if (byteSize == 0 || data == nullptr)
		{
			MGF_LOG_ERROR("GLPixelBuffer: Invalid data or byte size.");
			return false;
		}

		m_byteSize = byteSize;

		// 1. DSA 버퍼 생성
		glCreateBuffers(1, &m_handle);
		if (m_handle == 0)
		{
			MGF_LOG_ERROR("GLPixelBuffer: Failed to create buffer.");
			return false;
		}

		// 2. 데이터 업로드
		glNamedBufferData(m_handle, byteSize, data, GL_STREAM_DRAW);

		return false;
	}

	void GLPixelBuffer::Bind(Type type) const
	{
		if (m_handle == 0) return;
		uint32 target = CommonUtils::Select(type == Type::Pack, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER);
		glBindBuffer(target, m_handle);
	}

	void GLPixelBuffer::Unbind(Type type) const
	{
		uint32 target = CommonUtils::Select(type == Type::Pack, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER);
		glBindBuffer(target, 0);
	}

	void* GLPixelBuffer::Map(Access access)
	{
		if (m_handle == 0) return nullptr;

		uint32 glAccess = GL_WRITE_ONLY;
		switch (access)
		{
		case Access::ReadOnly: glAccess = GL_READ_ONLY; break;
		case Access::WriteOnly: glAccess = GL_WRITE_ONLY; break;
		case Access::ReadWrite: glAccess = GL_READ_WRITE; break;
		}

		return glMapNamedBuffer(m_handle, glAccess);
	}

	void GLPixelBuffer::Unmap()
	{
		if (m_handle == 0) return;
		glUnmapNamedBuffer(m_handle);
	}
}