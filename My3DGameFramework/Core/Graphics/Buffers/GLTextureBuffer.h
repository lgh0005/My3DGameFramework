#pragma once
#include "Graphics/Buffers/GLBufferHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureBuffer)
	class GLTextureBuffer : public GLBufferHandle
	{
	public:
		virtual ~GLTextureBuffer() override;
		static GLTextureBufferUPtr Create(uint32 format, const void* data, usize byteSize);
	
	public:
		void Bind(uint32 unit) const;
		void Unbind(uint32 unit) const;
		void UpdateData(const void* data, usize offset, usize size);
		uint32 GetTexture() const;

	private:
		GLTextureBuffer();
		bool Init(uint32 format, const void* data, usize byteSize);

		uint32 m_texture { 0 };
		uint32 m_format  { 0 };
	};
}