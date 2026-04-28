#include "GraphicsPch.h"
#include "GLUniformBuffer.h"

namespace MGF3D
{
	GLUniformBuffer::GLUniformBuffer() = default;
	GLUniformBuffer::~GLUniformBuffer() = default;

	GLUniformBufferUPtr GLUniformBuffer::Create(const void* data, usize byteSize)
	{
		auto buffer = GLUniformBufferUPtr(new GLUniformBuffer());
		if (!buffer->Init(data, byteSize)) return nullptr;
		return buffer;
	}

	bool GLUniformBuffer::Init(const void* data, usize byteSize)
	{
		if (byteSize == 0)
		{
			MGF_LOG_ERROR("GLUniformBuffer: Invalid byte size.");
			return false;
		}

		m_byteSize = byteSize;

		// 1. DSA: 타겟 지정 없이 버퍼 객체 생성
		glCreateBuffers(1, &m_handle);
		if (m_handle == 0)
		{
			MGF_LOG_ERROR("GLUniformBuffer: Failed to create buffer.");
			return false;
		}

		// 2. DSA: 메모리 할당 및 초기 데이터 업로드
		// UBO는 자주 갱신되므로 힌트로 GL_DYNAMIC_DRAW를 주는 것이 성능상 유리
		glNamedBufferData(m_handle, byteSize, data, GL_DYNAMIC_DRAW);

		return true;
	}

	void GLUniformBuffer::Bind(uint32 bindingIndex) const
	{
		if (m_handle == 0) return;
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, m_handle);
	}

	void GLUniformBuffer::BindToRange(uint32 bindingIndex, usize offset, usize size) const
	{
		if (m_handle == 0) return;
		glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, m_handle, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size));
	}

	void GLUniformBuffer::UpdateData(const void* data, usize offset, usize size)
	{
		if (m_handle == 0 || data == nullptr) return;
		if (offset + size > m_byteSize)
		{
			MGF_LOG_ERROR
			(
				"GLUniformBuffer: UpdateData bounds out of range. (Buffer Size: {0}, Req Range: {1}~{2})",
				m_byteSize, offset, offset + size
			);
			return;
		}

		// DSA: 바인딩 없이 직접 데이터를 밀어 넣습니다.
		glNamedBufferSubData(m_handle, offset, size, data);
	}

	void GLUniformBuffer::Unbind(uint32 bindingIndex)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, 0);
	}
}