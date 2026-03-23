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

	void GLVertexBuffer::Bind(uint32 vaoHandle, uint32 bindingIndex, uint32 stride, uint32 offset) const
	{
		if (m_allocation.IsValid())
		{
			uint64 totalOffset = m_allocation.GetOffset() + static_cast<uint64>(offset);
			glVertexArrayVertexBuffer(vaoHandle, bindingIndex, m_allocation.GetBufferHandle(), static_cast<GLintptr>(totalOffset), static_cast<GLsizei>(stride));
		}

	}

	void GLVertexBuffer::Unbind(uint32 vaoHandle, uint32 bindingIndex) const
	{
		glVertexArrayVertexBuffer(vaoHandle, bindingIndex, 0, 0, 0);
	}
}