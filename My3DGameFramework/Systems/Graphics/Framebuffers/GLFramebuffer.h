#pragma once
#include "Spatial/Rect.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLFramebuffer)
	class GLFramebuffer
	{
		MGF_DECLARE_CUSTOM_ALLOCATORS;

	public:
		virtual ~GLFramebuffer();
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Resize(uint32 w, uint32 h) = 0;

	public:
		virtual void Blit
		(
			const GLFramebufferPtr& src, 
			const GLFramebufferPtr& dst,
			const Ptr<Recti> srcRect = nullptr, 
			const Ptr<Recti> dstRect = nullptr,
			uint32 mask = GL_COLOR_BUFFER_BIT, 
			uint32 filter = GL_NEAREST
		) = 0;

	public:
		uint32 GetHandle() const;
		uint32 GetHash() const;
		uint32 ReleaseHandle();

	protected:
		GLFramebuffer();
		uint32 m_handle	{ 0 };
		uint32 m_hash	{ 0 };
	};
}