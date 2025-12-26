#pragma once
#include "FramebufferBase.h"

CLASS_PTR(BRDFLookUpFramebuffer)
class BRDFLookUpFramebuffer : public FramebufferBase
{
public:
	virtual ~BRDFLookUpFramebuffer();
	static BRDFLookUpFramebufferUPtr Create(int32 width, int32 height);

private:
	bool Init(int32 width, int32 height);
	BRDFLookUpFramebuffer();
};