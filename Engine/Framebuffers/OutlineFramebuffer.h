#pragma once
#include "Framebuffer.h"

CLASS_PTR(OutlineFramebuffer)
class OutlineFramebuffer : public Framebuffer
{
public:
	static OutlineFramebufferUPtr Create(int32 width, int32 height);
	virtual ~OutlineFramebuffer();

private:
	bool Init(int32 width, int32 height);
	OutlineFramebuffer();
};