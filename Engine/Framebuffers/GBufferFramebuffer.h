#pragma once
#include "Framebuffer.h"

CLASS_PTR(GBufferFramebuffer)
class GBufferFramebuffer : public Framebuffer
{
public:
	virtual ~GBufferFramebuffer();
	static GBufferFramebufferUPtr Create(int32 width, int32 height);

private:
	bool Init(int32 width, int32 height);
	GBufferFramebuffer();
};