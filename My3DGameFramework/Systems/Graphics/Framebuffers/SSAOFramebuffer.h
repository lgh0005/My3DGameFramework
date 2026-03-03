#pragma once
#include "Framebuffer.h"

namespace MGF3D
{
	CLASS_PTR(SSAOFramebuffer)
	class SSAOFramebuffer : public Framebuffer
	{
	public:
		virtual ~SSAOFramebuffer();
		static SSAOFramebufferUPtr Create(int32 width, int32 height);

	private:
		bool Init(int32 width, int32 height);
		SSAOFramebuffer();
		virtual bool CreateAttachments() override;
	};
}
