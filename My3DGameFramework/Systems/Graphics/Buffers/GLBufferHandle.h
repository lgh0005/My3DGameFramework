#pragma once

namespace MGF3D
{
	class GLBufferHandle
	{
	public:
		static void* operator new(usize size);
		static void operator delete(void* ptr, usize size);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

	protected:
		GLBufferHandle();
		virtual ~GLBufferHandle();

	protected:
		uint32 m_buffer	{ 0 };
	};
}