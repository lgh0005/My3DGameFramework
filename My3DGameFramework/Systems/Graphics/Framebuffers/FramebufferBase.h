#pragma once
#include "Resources/Resource.h"

CLASS_PTR(FramebufferBase)
class FramebufferBase
{
public:
	virtual ~FramebufferBase();
	const uint32 Get() const { return m_fbo; }
	
	void Bind() const;
	static void BindToDefault();
	virtual bool OnResize(int32 width, int32 height) = 0;
	static void Blit(FramebufferBase* src, FramebufferBase* dst, uint32 mask, uint32 filter);
	static void BlitRegion
	(
		FramebufferBase* src, int32 srcX0, int32 srcY0, int32 srcX1, int32 srcY1,
		FramebufferBase* dst, int32 dstX0, int32 dstY0, int32 dstX1, int32 dstY1,
		uint32 mask, uint32 filter
	);

	int32 GetWidth() const { return m_width; }
	int32 GetHeight() const { return m_height; }

	void Clean();

protected:
	FramebufferBase();
	bool CheckFramebufferStatus() const;

	uint32 m_fbo		{ 0 };
	int32 m_width		{ 0 };
	int32 m_height		{ 0 };
};