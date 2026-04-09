#include "CorePch.h"
#include "GLIndexBuffer.h"

namespace MGF3D
{
	GLIndexBuffer::GLIndexBuffer() = default;
	GLIndexBuffer::~GLIndexBuffer() = default;

	GLIndexBufferUPtr GLIndexBuffer::Create(const void* data, usize byteSize)
	{
		auto buffer = GLIndexBufferUPtr(new GLIndexBuffer());
		if (!buffer->Init(data, byteSize)) return nullptr;
		return buffer;
	}

	bool GLIndexBuffer::Init(const void* data, usize byteSize)
	{
		if (byteSize == 0 || data == nullptr)
		{
			MGF_LOG_ERROR("GLIndexBuffer: Invalid data or byte size.");
			return false;
		}

		m_byteSize = byteSize;

		// 1. DSA: 타겟 지정 없이 순수 버퍼 객체 생성
		glCreateBuffers(1, &m_handle);
		if (m_handle == 0)
		{
			MGF_LOG_ERROR("GLIndexBuffer: Failed to create buffer via glCreateBuffers.");
			return false;
		}

		// 2. DSA: 생성된 핸들에 직접 메모리 할당 및 데이터 업로드
		glNamedBufferStorage(m_handle, byteSize, data, 0);

		return true;
	}

	void GLIndexBuffer::Bind(uint32 vao)
	{
		if (m_handle == 0) return;
		glVertexArrayElementBuffer(vao, m_handle);
	}

	void GLIndexBuffer::Unbind(uint32 vao)
	{
		glVertexArrayElementBuffer(vao, 0);
	}
}