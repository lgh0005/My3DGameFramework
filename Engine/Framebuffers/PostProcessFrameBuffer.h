#pragma once
#include "Framebuffer.h"

CLASS_PTR(PostProcessFramebuffer)
class PostProcessFramebuffer : public Framebuffer
{
public:
	virtual ~PostProcessFramebuffer();
	static PostProcessFramebufferUPtr Create(int32 width, int32 height, bool useDepth);
	void AttachTexture(Texture* texture, int32 attachmentIndex = 0);

private:
	PostProcessFramebuffer();
	bool Init(int32 width, int32 height, bool useDepth);
};