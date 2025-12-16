#include "EnginePch.h"
#include "SkyLight.h"
#include "Resources/Texture.h"
#include "Resources/CubeTexture.h"

SkyLight::~SkyLight() = default;
SkyLight::SkyLight() = default;

SkyLightUPtr SkyLight::Create()
{
	return SkyLightUPtr(new SkyLight());
}

SkyLightUPtr SkyLight::CreateStandardSky(CubeTexturePtr skybox)
{
	auto sky = Create();
	sky->m_skybox = skybox;
	return std::move(sky);
}

SkyLightUPtr SkyLight::CreateUniversalSky(CubeTexturePtr skybox,  CubeTexturePtr irradiance, 
										   CubeTexturePtr prefilter, TexturePtr brdf)
{
	auto sky = Create();
	sky->m_skybox = skybox;
	sky->m_irradianceMap = irradiance;
	sky->m_prefilterMap = prefilter;
	sky->m_brdfLookUp = brdf;
	return std::move(sky);
}
