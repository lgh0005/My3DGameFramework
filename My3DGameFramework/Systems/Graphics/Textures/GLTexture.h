#pragma once
#include "Pointer/RefCount.h"
#include "Hashing/TextureHash.h"

namespace MGF3D
{
	class GLTexture : public RefCount
	{
	public:
		virtual ~GLTexture();
		virtual void Bind(uint32 slot) const = 0;
		virtual void Unbind(uint32 slot) const = 0;

	public:
		uint32 GetTarget() const;
		uint32 GetHandle() const;
		const TextureHash& GetHash() const;
		uint32 ReleaseHandle();

	protected:
		GLTexture();
		uint32		 m_target{ 0 };
		uint32		 m_handle	{ 0 };
		TextureHash  m_hash		{ 0 };
	};
}