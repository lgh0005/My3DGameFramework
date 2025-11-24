#include "EnginePch.h"
#include "Renderer.h"
#include "Core/Scene.h"
#include "Graphics/FrameBuffer.h"

Renderer::~Renderer() = default;

void Renderer::BlitCopyDepth(Framebuffer* src, Framebuffer* dst,
    int32 width,
    int32 height)
{
	if (!src || !dst) return;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->Get());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->Get());

	glBlitFramebuffer
	(
		0, 0, width, height,
		0, 0, width, height,
		GL_DEPTH_BUFFER_BIT,
		GL_NEAREST
	);
}