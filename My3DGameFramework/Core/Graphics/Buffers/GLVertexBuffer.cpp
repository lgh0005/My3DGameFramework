#include "CorePch.h"
#include "GLVertexBuffer.h"

namespace MGF3D
{
	GLVertexBuffer::GLVertexBuffer() = default;
	GLVertexBuffer::~GLVertexBuffer() = default;

	GLVertexBufferUPtr GLVertexBuffer::Create(const void* data, usize byteSize)
	{
		auto buffer = GLVertexBufferUPtr(new GLVertexBuffer());
		if (!buffer->Init(data, byteSize)) return nullptr;
		return buffer;
	}

	bool GLVertexBuffer::Init(const void* data, usize byteSize)
	{
		if (byteSize == 0 || data == nullptr)
		{
			MGF_LOG_ERROR("GLVertexBuffer: Invalid data or byte size.");
			return false;
		}

		m_byteSize = byteSize;

		// 1. DSA: 타겟 바인딩 없이 버퍼 객체 생성
		glCreateBuffers(1, &m_handle);
		if (m_handle == 0)
		{
			MGF_LOG_ERROR("GLVertexBuffer: Failed to create buffer via glCreateBuffers.");
			return false;
		}

		// 2. DSA: 생성된 핸들에 직접 메모리 할당 및 데이터 업로드
		glNamedBufferData(m_handle, byteSize, data, GL_STATIC_DRAW);

		return true;
	}

	void GLVertexBuffer::Bind(uint32 vao, uint32 bindingIndex, uint32 stride, uint32 offset)
	{
		if (m_handle == 0) return;
		glVertexArrayVertexBuffer(vao, bindingIndex, m_handle, static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));
	}

	void GLVertexBuffer::Unbind(uint32 vao, uint32 bindingIndex)
	{
		glVertexArrayVertexBuffer(vao, bindingIndex, 0, 0, 0);
	}
}