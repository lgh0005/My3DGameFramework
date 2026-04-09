#pragma once
#include "Graphics/Buffers/GLBufferHandle.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLPixelBuffer)
	class GLPixelBuffer : public GLBufferHandle
	{
	public:

		// PBO의 데이터 전송 방향
		enum class Type
		{
			Unpack, // CPU -> GPU (텍스처 업로드용)
			Pack    // GPU -> CPU (화면 캡처, 데이터 리드백용)
		};

		// 메모리 매핑 권한
		enum class Access
		{
			ReadOnly,
			WriteOnly,
			ReadWrite
		};

	public:
		virtual ~GLPixelBuffer() override;
		static GLPixelBufferUPtr Create(const void* data, usize byteSize);

	public:
		void Bind(Type type) const;
		void Unbind(Type type) const;
		void* Map(Access access);
		void Unmap();

	private:
		GLPixelBuffer();
		bool Init(const void* data, usize byteSize);
	};
}