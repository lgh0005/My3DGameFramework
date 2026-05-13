#pragma once
#include "Sources/Resource.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLTextureArrayHandle)
	class GLTextureArrayHandle : public Resource
	{
		MGF_DISABLE_COPY(GLTextureArrayHandle)

	public:
		virtual bool OnSyncCreate() override = 0;
		void Bind(uint32 slot) const;
		static void Unbind(uint32 slot);

	public:
		uint32 GetTarget() const { return m_target; }
		uint32 GetHandle() const { return m_handle; }
		uint32 GetWidth() const { return m_width; }
		uint32 GetHeight() const { return m_height; }
		uint32 GetLayers() const { return m_layers; }

	protected:
		GLTextureArrayHandle();
		virtual ~GLTextureArrayHandle();

	protected:
		uint32 m_target{ 0 };
		uint32 m_handle{ 0 };

		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
		uint32 m_layers{ 0 };
	};
}