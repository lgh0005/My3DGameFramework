#pragma once
#include "Framebuffer.h"

CLASS_PTR(PostProcessFramebuffer)
class PostProcessFramebuffer : public Framebuffer
{
public:
	virtual ~PostProcessFramebuffer();
	static PostProcessFramebufferUPtr Create(int32 width, int32 height, bool useDepth);
	void AttachTexture(Texture* texture, int32 attachmentIndex = 0);

// [TEMP]
public:
	static void Blit
	(
		PostProcessFramebuffer* src, PostProcessFramebuffer* dst,
		int32 width, int32 height,
		uint32 mask = GL_COLOR_BUFFER_BIT,
		uint32 filter = GL_NEAREST
	);

private:
	PostProcessFramebuffer();
	bool Init(int32 width, int32 height, bool useDepth);
};