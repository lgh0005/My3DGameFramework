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

	void GLUniformBuffer::Bind(uint32 uboSlot) const
	{
		if (m_allocation.IsValid())
		{
			// 서브 할당된 VRAM 구간(오프셋과 사이즈)만 정확하게 셰이더 슬롯에 바인딩
			glBindBufferRange
			(
				GL_UNIFORM_BUFFER,
				uboSlot,
				m_allocation.GetBufferHandle(),
				static_cast<GLintptr>(m_allocation.GetOffset()),
				static_cast<GLsizeiptr>(m_allocation.GetSize())
			);
		}
	}

	void GLUniformBuffer::Unbind(uint32 uboSlot) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, uboSlot, 0);
	}

	bool GLUniformBuffer::Init(usize byteSize, VRAMAllocation::PoolType poolType)
	{
		if (byteSize == 0) return false;

		m_allocation = MGF_VRAM.Allocate(poolType, byteSize, 256);
		if (!m_allocation.IsValid())
		{
			MGF_LOG_ERROR("GLUniformBuffer: Failed to allocate VRAM.");
			return false;
		}

		return true;
	}
}