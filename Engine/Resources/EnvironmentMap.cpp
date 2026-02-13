#include "EnginePch.h"
#include "EnvironmentMap.h"
#include "Resources/Image.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Textures/CubeTexture.h"
#include "Graphics/IBLUtils.h"

DECLARE_DEFAULTS_IMPL(EnvironmentMap)

EnvironmentMapPtr EnvironmentMap::Load(const EnvironmentMapDesc& desc)
{
    // 1. 리소스 인스턴스 생성
    EnvironmentMapPtr envMap(new EnvironmentMap());
    envMap->SetPath(desc.path);
    envMap->SetName(desc.name);

    // 2. 확장자 확인 (.hdr vs .ktx/.dds)
    // TODO : 역시 확장자를 구체적으로 정해줘야함.
    std::string ext = std::filesystem::path(desc.path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".hdr")
    {
        TextureDesc hdrDesc(desc.path);
        auto hdrTexture = RESOURCE.Load<Texture>(hdrDesc);

        if (hdrTexture)
        {
            // 2D 텍스처를 큐브맵으로 변환
            envMap->m_skybox = IBLUtils::CreateCubemapFromHDR(hdrTexture.get());
        }
        else
        {
            LOG_ERROR("EnvironmentMap: Failed to load HDR texture: {}", desc.path);
            return nullptr;
        }
    }
    else
    {
        CubeTextureDesc cubeDesc(desc.path);
        envMap->m_skybox = RESOURCE.Load<CubeTexture>(cubeDesc);

        if (!envMap->m_skybox)
        {
            LOG_ERROR("EnvironmentMap: Failed to load CubeTexture: {}", desc.path);
            return nullptr;
        }
    }

    // 3. IBL 데이터 생성
    if (desc.createIBL && envMap->m_skybox)
    {
        envMap->m_irradiance = IBLUtils::CreateIrradianceMap(envMap->m_skybox.get());
        envMap->m_prefiltered = IBLUtils::CreatePrefilteredMap(envMap->m_skybox.get());
        envMap->m_brdf = IBLUtils::CreateBRDFLUT();

        LOG_INFO("EnvironmentMap Loaded (Full IBL): {}", desc.path);
    }
    else
    {
        LOG_INFO("EnvironmentMap Loaded (Skybox Only): {}", desc.path);
    }


    return envMap;
}

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