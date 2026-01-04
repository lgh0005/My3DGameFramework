#include "EnginePch.h"
#include "EnvironmentMap.h"
#include "Resources/Image.h"
#include "Resources/Texture.h"
#include "Resources/CubeTexture.h"
#include "Misc/IBLUtils.h"

DECLARE_DEFAULTS_IMPL(EnvironmentMap)

EnvironmentMapUPtr EnvironmentMap::Create(const CubeTexturePtr& cubeTexture)
{
    // 1. 일반 스카이맵 생성
    auto envMap = EnvironmentMapUPtr(new EnvironmentMap());
    envMap->m_skybox = cubeTexture;
    return std::move(envMap);
}

EnvironmentMapUPtr EnvironmentMap::CreateIBL(const TexturePtr& hdrTexture)
{
    // EnvironmentMap 리소스 생성
    auto envMap = EnvironmentMapUPtr(new EnvironmentMap());
    envMap->m_skybox      = IBLUtils::CreateCubemapFromHDR(hdrTexture.get());
    envMap->m_irradiance  = IBLUtils::CreateIrradianceMap(envMap->m_skybox.get());
    envMap->m_prefiltered = IBLUtils::CreatePrefilteredMap(envMap->m_skybox.get());
    envMap->m_brdf        = IBLUtils::CreateBRDFLUT();
    return std::move(envMap);
}