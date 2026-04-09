#pragma once

namespace MGF3D
{
	class GLTextureHandle
	{
	public:
		void Bind(uint32 slot) const;
		void Unbind(uint32 slot) const;

	public:
		uint32 GetTarget() const;
		uint32 GetHandle() const;
		void GenerateMipmap();

	protected:
		GLTextureHandle();
		virtual ~GLTextureHandle();

	protected:
		uint32 m_target{ 0 };
		uint32 m_handle{ 0 };
	};
}