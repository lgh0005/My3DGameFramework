#include "GraphicsPch.h"
#include "GLVertexBuffer.h"
#include "Managers/VRAMManager.h"

namespace MGF3D
{
	GLVertexBuffer::GLVertexBuffer() = default;
	GLVertexBuffer::~GLVertexBuffer() = default;

	GLVertexBufferUPtr GLVertexBuffer::Create(const void* data, usize byteSize, VRAMAllocation::PoolType poolType)
	{
		auto buffer = GLVertexBufferUPtr(new GLVertexBuffer());
		if (!buffer->Init(data, byteSize, poolType)) return nullptr;
		return buffer;
	}

	bool GLVertexBuffer::Init(const void* data, usize byteSize, VRAMAllocation::PoolType poolType)
	{
		if (byteSize == 0 || data == nullptr) return false;

		m_allocation = MGF_VRAM.Allocate(poolType, byteSize, 16);
		if (!m_allocation.IsValid())
		{
			MGF_LOG_ERROR("GLVertexBuffer: Failed to allocate VRAM.");
			return false;
		}

		MGF_VRAM.UploadData(m_allocation, data);
		return true;
	}

	void GLVertexBuffer::Bind() const
	{
		if (m_allocation.IsValid())
			glBindBuffer(GL_ARRAY_BUFFER, m_allocation.GetBufferHandle());
	}

	void GLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}