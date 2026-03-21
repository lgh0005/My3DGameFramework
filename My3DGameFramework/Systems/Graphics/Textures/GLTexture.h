#pragma once
#include "VRAM/VRAMAllocation.h"

namespace MGF3D
{
	class GLTexture
	{
		MGF_DECLARE_CUSTOM_ALLOCATORS;

	public:
		virtual ~GLTexture();
		virtual void Bind(uint32 slot) = 0;
		virtual void Unbind(uint32 slot) = 0;
		uint32 GetTarget() const;

	public:
		uint32 GetHandle() const;
		uint32 ReleaseHandle();

	protected:
		GLTexture();
		uint32 m_handle		{ 0 };
		uint32 m_target		{ 0 };
	};
}