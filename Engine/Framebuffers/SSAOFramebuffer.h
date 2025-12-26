#pragma once
#include "FramebufferBase.h"

CLASS_PTR(SSAOFramebuffer)
class SSAOFramebuffer : public FramebufferBase
{
public:
	virtual ~SSAOFramebuffer();
	static SSAOFramebufferUPtr Create(int32 width, int32 height);

private:
	bool Init(int32 width, int32 height);
	SSAOFramebuffer();
};