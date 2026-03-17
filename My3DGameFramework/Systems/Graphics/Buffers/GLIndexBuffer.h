#pragma once
#include "Buffers/GLBufferHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLIndexBuffer)
	class GLIndexBuffer : public GLBufferHandle
	{
	public:
		virtual ~GLIndexBuffer() override;
		static GLIndexBufferUPtr Create
		(
			const void* data,
			usize		byteSize,
			uint32		flags = 0
		);

	public:
		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		GLIndexBuffer();
		bool Init
		(
			const void* data,
			usize		byteSize,
			uint32		flags = 0
		);
	};
}