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
		auto buffer = GLIndexBufferUPtr(new GLIndexBuffer());
		if (!buffer->Init(data, byteSize, poolType)) return nullptr;
		return buffer;
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

	void GLIndexBuffer::Bind(uint32 vaoHandle) const
	{
		if (m_allocation.IsValid())
			glVertexArrayElementBuffer(vaoHandle, m_allocation.GetBufferHandle());
	}

	void GLIndexBuffer::Unbind(uint32 vaoHandle) const
	{
		glVertexArrayElementBuffer(vaoHandle, 0);
	}
}