#include "FrameworkPch.h"
#include "MaterialUtils.h"
#include "Textures/GLTexture2D.h"
#include "Managers/ResourceManager.h"

namespace MGF3D
{
	GLTexture2DPtr MaterialUtils::s_defaultAlbedo = nullptr;
	GLTexture2DPtr MaterialUtils::s_defaultNormal = nullptr;
	GLTexture2DPtr MaterialUtils::s_defaultORM = nullptr;
	GLTexture2DPtr MaterialUtils::s_defaultEmission = nullptr;

	GLTexture2D* MaterialUtils::GetDefaultAlbedo()
	{
		if (!s_defaultAlbedo) s_defaultAlbedo = CreateDummyTexture(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		return s_defaultAlbedo.get();
	}

	GLTexture2D* MaterialUtils::GetDefaultNormal()
	{
		if (!s_defaultNormal) s_defaultNormal = CreateDummyTexture(vec4(0.5f, 0.5f, 1.0f, 1.0f));
		return s_defaultNormal.get();
	}

	GLTexture2D* MaterialUtils::GetDefaultORM()
	{
		if (!s_defaultORM) s_defaultORM = CreateDummyTexture(vec4(1.0f, 1.0f, 0.0f, 1.0f));
		return s_defaultORM.get();
	}

	GLTexture2D* MaterialUtils::GetDefaultEmission()
	{
		if (!s_defaultEmission) s_defaultEmission = CreateDummyTexture(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		return s_defaultEmission.get();
	}

	GLTexture2DPtr MaterialUtils::CreateDummyTexture(const vec4& color)
	{
		auto tex = MGF_RESOURCE.CreateImmediate<GLTexture2D>(1, 1, GL_RGBA8, 1);
		tex->SetFilter(GL_NEAREST, GL_NEAREST);
		tex->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		tex->UpdateSubData(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color[0]);
		return tex;
	}
}