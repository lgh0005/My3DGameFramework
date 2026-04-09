#pragma once

namespace MGF3D
{
	class GLBufferHandle
	{
	public:
		uint32 GetHandle() const;
		usize GetByteSize() const;

	protected:
		GLBufferHandle();
		virtual ~GLBufferHandle();

	protected:
		uint32 m_handle{ 0 };
		usize m_byteSize{ 0 };
	};
}