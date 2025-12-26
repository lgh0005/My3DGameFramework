#pragma once
#include "FramebufferBase.h"

CLASS_PTR(PostProcessFramebuffer)
class PostProcessFramebuffer : public FramebufferBase
{
public:
	virtual ~PostProcessFramebuffer();
	static PostProcessFramebufferUPtr Create(int32 width, int32 height, bool useDepth);
	void AttachTexture(Texture* texture, int32 attachmentIndex);

private:
	PostProcessFramebuffer();
	bool Init(int32 width, int32 height, bool useDepth);
};