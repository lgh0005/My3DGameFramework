#pragma once
#include "Buffers/GLBufferHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLUniformBuffer)
	class GLUniformBuffer : public GLBufferHandle
	{
	public:
		virtual ~GLUniformBuffer() override;
		static GLUniformBufferUPtr Create(const void* data, usize byteSize);

	public:
		void Bind(uint32 bindingIndex) const;
		void BindToRange(uint32 bindingIndex, usize offset, usize size) const;
		void UpdateData(const void* data, usize offset, usize size);
		void Unbind(uint32 bindingIndex) const;

	private:
		GLUniformBuffer();
		bool Init(const void* data, usize byteSize);
	};
}