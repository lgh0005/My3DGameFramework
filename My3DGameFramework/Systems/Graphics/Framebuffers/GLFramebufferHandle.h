#pragma once

namespace MGF3D
{
	class GLFramebufferHandle
	{
	public:
		void Bind(uint32 target	  = GL_FRAMEBUFFER) const;
		void Unbind(uint32 target = GL_FRAMEBUFFER) const;
		static void Blit
		(
			const GLFramebufferHandle* src, const GLFramebufferHandle* dst,
			int32 srcX0, int32 srcY0, int32 srcX1, int32 srcY1,
			int32 dstX0, int32 dstY0, int32 dstX1, int32 dstY1,
			uint32 mask, uint32 filter
		);

	public:
		uint32 GetTarget() const;
		uint32 GetHandle() const;

	protected:
		GLFramebufferHandle();
		virtual ~GLFramebufferHandle();
		bool CheckStatus(uint32 target = GL_FRAMEBUFFER) const;

	protected:
		uint32 m_target{ 0 };
		uint32 m_handle{ 0 };
	};
}