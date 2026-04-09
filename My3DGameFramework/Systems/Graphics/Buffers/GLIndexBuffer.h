#pragma once
#include "Graphics/Buffers/GLBufferHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLIndexBuffer)
	class GLIndexBuffer : public GLBufferHandle
	{
	public:
		virtual ~GLIndexBuffer() override;
		static GLIndexBufferUPtr Create(const void* data, usize byteSize);

	public:
		void Bind(uint32 vao);
		void Unbind(uint32 vao);

	private:
		GLIndexBuffer();
		bool Init(const void* data, usize byteSize);
	};
}