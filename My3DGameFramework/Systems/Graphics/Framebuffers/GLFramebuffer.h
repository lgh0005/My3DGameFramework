#pragma once
#include "Pointer/RefCount.h"
#include "Spatial/Rect.h"

namespace MGF3D
{
	enum class FramebufferType : uint32
	{
		NONE			 = 0,
		FRAMEBUFFER_2D   = 1,
		FRAMEBUFFER_3D   = 2,
		FRAMEBUFFER_CUBE = 3
	};

	MGF_CLASS_PTR(GLFramebuffer)
	class GLFramebuffer : public RefCount
	{
	public:
		virtual ~GLFramebuffer();
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

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
		FramebufferType GetTarget() const;
		uint32			GetHandle() const;
		uint32			GetHash() const;
		uint32			ReleaseHandle();

	protected:
		GLFramebuffer();
		FramebufferType m_type	     { FramebufferType::NONE };
		uint32	        m_handle	 { 0 };
		uint32	        m_hash		 { 0 };
	};
}