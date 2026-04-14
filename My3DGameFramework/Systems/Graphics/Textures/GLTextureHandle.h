#pragma once
#include "Sources/Resource.h"

struct ktxTexture2;

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureHandle)
	class GLTextureHandle : public Resource
	{
	public:
		virtual bool OnSyncCreate() override = 0;
		void Bind(uint32 slot) const;
		void Unbind(uint32 slot) const;

	/*==========================//
	//   GLTextureHandle Type   //
	//==========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		uint32 GetTarget() const;
		uint32 GetHandle() const;
		void GenerateMipmap();

	public:
		void SetKtxTexture(ktxTexture2* ktx) { m_ktxTexture = ktx; }
		ktxTexture2* GetTexture() const { return m_ktxTexture; }

	protected:
		GLTextureHandle();
		virtual ~GLTextureHandle();

	protected:
		uint32 m_target{ 0 };
		uint32 m_handle{ 0 };
		ktxTexture2* m_ktxTexture { nullptr };
	};
}