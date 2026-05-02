#include "GraphicsPch.h"
#include "GLShaderStorageBuffer.h"

namespace MGF3D
{
	GLShaderStorageBuffer::GLShaderStorageBuffer() = default;
	GLShaderStorageBuffer::~GLShaderStorageBuffer() = default;

	GLShaderStorageBufferUPtr GLShaderStorageBuffer::Create(const void* data, usize byteSize)
	{
		auto buffer = GLShaderStorageBufferUPtr(new GLShaderStorageBuffer());
		if (!buffer->Init(data, byteSize)) return nullptr;
		return buffer;
	}

	bool GLShaderStorageBuffer::Init(const void* data, usize byteSize)
	{
		m_byteSize = byteSize;

		// 1. 타겟 지정 없이 버퍼 객체 생성
		glCreateBuffers(1, &m_handle);
		if (m_handle == 0)
		{
			MGF_LOG_ERROR("GLShaderStorageBuffer: Failed to create buffer.");
			return false;
		}

		// 2. 메모리 할당 및 초기 데이터 업로드
		glNamedBufferData(m_handle, byteSize, data, GL_DYNAMIC_DRAW);

		return true;
	}

	void GLShaderStorageBuffer::Bind(uint32 bindingIndex) const
	{
		if (m_handle == 0) return;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, m_handle);
	}

	void GLShaderStorageBuffer::BindToRange(uint32 bindingIndex, usize offset, usize size) const
	{
		if (m_handle == 0) return;
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingIndex, m_handle, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size));
	}

	void GLShaderStorageBuffer::UpdateData(const void* data, usize offset, usize size)
	{
		if (m_handle == 0 || data == nullptr) return;
		if (offset + size > m_byteSize)
		{
			MGF_LOG_ERROR
			(
				"GLShaderStorageBuffer: UpdateData bounds out of range. (Buffer Size: {0}, Req Range: {1}~{2})",
				m_byteSize, offset, offset + size
			);
			return;
		}

		// DSA: 바인딩 없이 직접 데이터를 밀어 넣습니다.
		glNamedBufferSubData(m_handle, offset, size, data);
	}

	void GLShaderStorageBuffer::Unbind(uint32 bindingIndex)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, 0);
	}
}