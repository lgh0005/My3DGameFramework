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
		virtual void Bind() const override;
		virtual void Unbind() const override;

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