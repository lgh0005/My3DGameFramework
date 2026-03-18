#include "GraphicsPch.h"
#include "GLIndexBuffer.h"
#include "Managers/VRAMManager.h"

namespace MGF3D
{
	GLIndexBuffer::GLIndexBuffer() = default;
	GLIndexBuffer::~GLIndexBuffer() = default;

	GLIndexBufferUPtr GLIndexBuffer::Create
	(
		const void* data, 
		usize byteSize,
		VRAMAllocation::PoolType poolType
	)
	{
		auto indexBuffer = GLIndexBufferUPtr(new GLIndexBuffer());
		if (!indexBuffer->Init(data, byteSize, poolType)) return nullptr;
		return std::move(indexBuffer);
	}

	bool GLIndexBuffer::Init(const void* data, usize byteSize, VRAMAllocation::PoolType poolType)
	{
		if (byteSize == 0 || data == nullptr) return false;

		m_allocation = MGF_VRAM.Allocate(poolType, byteSize, 4);
		if (!m_allocation.IsValid())
		{
			MGF_LOG_ERROR("GLIndexBuffer: Failed to allocate VRAM.");
			return false;
		}

		MGF_VRAM.UploadData(m_allocation, data);
		return true;
	}

	void GLIndexBuffer::Bind() const
	{
		if (m_allocation.IsValid())
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_allocation.GetBufferHandle());
	}

	void GLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}