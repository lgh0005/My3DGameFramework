#pragma once

namespace MGF3D
{
	class GLBufferHandle
	{
		MGF_DECLARE_CUSTOM_ALLOCATORS();

	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

	protected:
		GLBufferHandle();
		virtual ~GLBufferHandle();

	protected:
		uint32 m_buffer	{ 0 };
	};
}