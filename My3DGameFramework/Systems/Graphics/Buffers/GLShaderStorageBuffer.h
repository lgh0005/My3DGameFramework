#pragma once
#include "Buffers/GLBufferHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLShaderStorageBuffer)
	class GLShaderStorageBuffer : public GLBufferHandle
	{
	public:
		virtual ~GLShaderStorageBuffer() override;
		static GLShaderStorageBufferUPtr Create(const void* data, usize byteSize);

	public:
		void Bind(uint32 bindingIndex) const;
		void BindToRange(uint32 bindingIndex, usize offset, usize size) const;
		void UpdateData(const void* data, usize offset, usize size);
		void Unbind(uint32 bindingIndex) const;

	private:
		GLShaderStorageBuffer();
		bool Init(const void* data, usize byteSize);
	};
}