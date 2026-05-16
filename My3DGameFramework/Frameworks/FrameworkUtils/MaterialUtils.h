#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(GLTexture2D)

	class MaterialUtils
	{
		MGF_DECLARE_UTILITIES(MaterialUtils)

	public:
		static GLTexture2D* GetDefaultAlbedo();
		static GLTexture2D* GetDefaultNormal();
		static GLTexture2D* GetDefaultORM();
		static GLTexture2D* GetDefaultEmission();

	private:
		static GLTexture2DPtr s_defaultAlbedo;
		static GLTexture2DPtr s_defaultNormal;
		static GLTexture2DPtr s_defaultORM;
		static GLTexture2DPtr s_defaultEmission;

		static GLTexture2DPtr CreateDummyTexture(const vec4& color);
	};
}