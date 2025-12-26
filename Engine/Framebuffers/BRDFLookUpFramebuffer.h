#pragma once
#include "Framebuffer.h"

CLASS_PTR(BRDFLookUpFramebuffer)
class BRDFLookUpFramebuffer : public Framebuffer
{
public:
	virtual ~BRDFLookUpFramebuffer();
	static BRDFLookUpFramebufferUPtr Create(int32 width, int32 height);

private:
	bool Init(int32 width, int32 height);
	BRDFLookUpFramebuffer();
};