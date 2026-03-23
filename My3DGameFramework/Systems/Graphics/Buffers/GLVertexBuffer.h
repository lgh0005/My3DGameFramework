#pragma once
#include "GLBufferHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLVertexBuffer)
	class GLVertexBuffer : public GLBufferHandle
	{
	public:
		virtual ~GLVertexBuffer() override;
		static GLVertexBufferUPtr Create
		(
			const void* data, 
			usize		byteSize, 
			VRAMAllocation::PoolType poolType = VRAMAllocation::PoolType::Static
		);

	public:
		void Bind(uint32 vaoHandle, uint32 bindingIndex, uint32 stride, uint32 offset = 0) const;
		void Unbind(uint32 vaoHandle, uint32 bindingIndex) const;

	private:
		GLVertexBuffer();
		bool Init
		(
			const void* data, 
			usize		byteSize, 
			VRAMAllocation::PoolType poolType
		);
	};
}