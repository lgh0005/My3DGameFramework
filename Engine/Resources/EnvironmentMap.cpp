#include "EnginePch.h"
#include "EnvironmentMap.h"
#include "Resources/Image.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Textures/CubeTexture.h"
#include "Graphics/IBLUtils.h"

DECLARE_DEFAULTS_IMPL(EnvironmentMap)

EnvironmentMapPtr EnvironmentMap::Load(const EnvironmentMapDesc& desc)
{
    // 1. 리소스 인스턴스 생성 및 이름 설정
    EnvironmentMapPtr envMap(new EnvironmentMap());
    envMap->m_desc = desc;

    // 2. 확장자 확인
    std::string ext = std::filesystem::path(desc.path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".hdr")
    {
        // RESOURCE.Add를 사용하여 HDR 텍스처 로드 (캐싱 및 가상 경로 지원)
        // 이름은 리소스 구분을 위해 path를 기반으로 생성하거나 별도 규칙 적용
        auto hdrTexture = RESOURCE.Add<Texture>(desc.path, desc.path);

        if (hdrTexture)
        {
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
        // .ktx 등 큐브맵 파일 직접 로드
        envMap->m_skybox = RESOURCE.Add<CubeTexture>(desc.path, desc.path);

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