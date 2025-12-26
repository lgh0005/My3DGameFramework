#pragma once
#include "FramebufferBase.h"

CLASS_PTR(GBufferFramebuffer)
class GBufferFramebuffer : public FramebufferBase
{
public:
	virtual ~GBufferFramebuffer();
	static GBufferFramebufferUPtr Create(int32 width, int32 height);

private:
	bool Init(int32 width, int32 height);
	GBufferFramebuffer();
};