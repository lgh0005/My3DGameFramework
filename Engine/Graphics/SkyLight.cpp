#include "EnginePch.h"
#include "SkyLight.h"
#include "Resources/Texture.h"
#include "Resources/CubeTexture.h"
#include "Resources/EnvironmentMap.h"

DECLARE_DEFAULTS_IMPL(SkyLight)

SkyLightUPtr SkyLight::Create(const EnvironmentMapPtr& envMap)
{
	auto skyLight = SkyLightUPtr(new SkyLight());
	if (!skyLight->Init(envMap)) return nullptr;
	return std::move(skyLight);
}

bool SkyLight::Init(const EnvironmentMapPtr& envMap)
{
	m_environmentMap = envMap;
	if (!m_environmentMap) return false;
	return true;
}

CubeTexture* SkyLight::GetSkybox() const
{
	auto skybox = m_environmentMap->GetSkybox();
	if (skybox) return skybox.get();
	return nullptr;
}

CubeTexture* SkyLight::GetIrradianceMap() const
{
	auto irradiance = m_environmentMap->GetIrradiance();
	if (irradiance) return irradiance.get();
	return nullptr;
}

CubeTexture* SkyLight::GetPrefilterMap() const
{
	auto prefilter = m_environmentMap->GetPrefiltered();
	if (prefilter) return prefilter.get();
	return nullptr;
}

Texture* SkyLight::GetBRDFLookUp() const
{
	auto brdf = m_environmentMap->GetBrdfLUT();
	if (brdf) return brdf.get();
	return nullptr;
}
