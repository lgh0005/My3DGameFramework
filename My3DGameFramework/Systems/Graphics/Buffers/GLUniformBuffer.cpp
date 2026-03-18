#include "GraphicsPch.h"
#include "GLUniformBuffer.h"
#include "Managers/VRAMManager.h"

namespace MGF3D
{
	GLUniformBuffer::GLUniformBuffer() = default;
	GLUniformBuffer::~GLUniformBuffer() = default;

	GLUniformBufferUPtr GLUniformBuffer::Create(usize byteSize, VRAMAllocation::PoolType poolType)
	{
		auto buffer = GLUniformBufferUPtr(new GLUniformBuffer());
		if (!buffer->Init(byteSize, poolType)) return nullptr;
		return buffer;
	}

	bool GLUniformBuffer::Init(usize byteSize, VRAMAllocation::PoolType poolType)
	{
		if (byteSize == 0) return false;

		// TODO : 이건 모듈 실행 초기에 어딘가에서 받아와서 전역적으로 사용할 수 있도록
		// 보장을 해줘야 함.
		//GLint minUboAlignment = 0;
		//glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &minUboAlignment);

		m_allocation = MGF_VRAM.Allocate(poolType, byteSize, 256);
		if (!m_allocation.IsValid())
		{
			MGF_LOG_ERROR("GLUniformBuffer: Failed to allocate VRAM.");
			return false;
		}

		return true;
	}

	void GLUniformBuffer::Bind() const
	{
		if (m_allocation.IsValid())
			glBindBuffer(GL_UNIFORM_BUFFER, m_allocation.GetBufferHandle());
	}

	void GLUniformBuffer::BindBase(uint32 slot) const
	{
		if (m_allocation.IsValid())
		{
			glBindBufferRange
			(
				GL_UNIFORM_BUFFER,
				slot,
				m_allocation.GetBufferHandle(),
				m_allocation.GetOffset(),
				m_allocation.GetSize()
			);
		}
	}

	void GLUniformBuffer::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}