#pragma once
#include "Graphics/Buffers/GLBufferHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLVertexBuffer)
	class GLVertexBuffer : public GLBufferHandle
	{
	public:
		virtual ~GLVertexBuffer() override;
		static GLVertexBufferUPtr Create(const void* data, usize byteSize);
	
	public:
		void Bind(uint32 vao, uint32 bindingIndex, uint32 stride, uint32 offset = 0);
		void Unbind(uint32 vao, uint32 bindingIndex);

	private:
		GLVertexBuffer();
		bool Init(const void* data, usize byteSize);
	};
}