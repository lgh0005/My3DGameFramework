#include "EnginePch.h"
#include "EnvironmentMap.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Textures/CubeTexture.h"
#include "Graphics/IBLUtils.h"

DECLARE_DEFAULTS_IMPL(EnvironmentMap)

EnvironmentMapPtr EnvironmentMap::Load(const EnvironmentMapDesc& desc)
{
    EnvironmentMapPtr envMap(new EnvironmentMap());
    envMap->m_desc = desc;

    // 1. 먼저 스카이박스(CubeTexture)인지 탐색
    envMap->m_skybox = RESOURCE.Get<CubeTexture>(desc.name);

    // 2. 과정 1에서 CubeTexture가 없다면 .hdr로 등록된 텍스쳐이고 이걸로 IBL을 만들어야 함
    if (!envMap->m_skybox)
    {
        auto hdrTex = RESOURCE.Get<Texture>(desc.name);
        if (hdrTex)
        {
            envMap->m_skybox = IBLUtils::CreateCubemapFromHDR(hdrTex.get());
            envMap->m_irradiance = IBLUtils::CreateIrradianceMap(envMap->m_skybox.get());
            envMap->m_prefiltered = IBLUtils::CreatePrefilteredMap(envMap->m_skybox.get());
            envMap->m_brdf = IBLUtils::CreateBRDFLUT();
        }
    }

    // 3. 스카이 박스 최종 검사
    if (!envMap->m_skybox)
    {
        LOG_ERROR("EnvironmentMap: Failed to find any source (Cube/HDR) for [{}]", desc.name);
        return nullptr;
    }

    return envMap;
}

EnvironmentMapPtr EnvironmentMap::Create(const CubeTexturePtr& cubeTexture)
{
    if (!cubeTexture) return nullptr;
    EnvironmentMapPtr envMap(new EnvironmentMap());
    envMap->m_skybox = cubeTexture;
    envMap->m_desc.name = cubeTexture->GetName();
    return envMap;
}

EnvironmentMapPtr EnvironmentMap::CreateIBL(const TexturePtr& hdrTexture)
{
    if (!hdrTexture) return nullptr;

    EnvironmentMapPtr envMap(new EnvironmentMap());

    // IBL 베이킹 프로세스
    envMap->m_skybox = IBLUtils::CreateCubemapFromHDR(hdrTexture.get());
    envMap->m_irradiance = IBLUtils::CreateIrradianceMap(envMap->m_skybox.get());
    envMap->m_prefiltered = IBLUtils::CreatePrefilteredMap(envMap->m_skybox.get());
    envMap->m_brdf = IBLUtils::CreateBRDFLUT();
    envMap->m_desc.name = hdrTexture->GetName();
    return envMap;
}