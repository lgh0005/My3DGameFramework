#pragma once
#include "Framebuffer.h"

CLASS_PTR(EffectFramebuffer)
class EffectFramebuffer : public Framebuffer
{
public:
	virtual ~EffectFramebuffer();
	static EffectFramebufferUPtr Create(int32 width, int32 height);
	static EffectFramebufferUPtr CreateEmpty();

private:
	EffectFramebuffer();
	virtual bool CreateAttachments() override;
	bool Init(int32 width, int32 height);
	bool InitEmpty();

	bool m_isEmpty = false;
};