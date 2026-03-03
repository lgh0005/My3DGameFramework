#pragma once
#include "Framebuffer.h"

namespace MGF3D
{
	CLASS_PTR(PostProcessFramebuffer)
	class PostProcessFramebuffer : public Framebuffer
	{
	public:
		virtual ~PostProcessFramebuffer();
		static PostProcessFramebufferUPtr Create(int32 width, int32 height);

	private:
		PostProcessFramebuffer();
		bool Init(int32 width, int32 height);
		virtual bool CreateAttachments() override;
	};
}
