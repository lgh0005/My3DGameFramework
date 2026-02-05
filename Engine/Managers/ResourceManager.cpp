#include "EnginePch.h"
#include "ResourceManager.h"
#include "Core/ResourceLoader.h"

bool ResourceManager::Init()
{
    // 1. 빌트인 리소스(Cube, Plane, Material 등) 생성
    if (!ResourceLoader::Init()) 
        return false;

    // 2. 기본 엔진 셰이더 로드
    std::string shaderConfigPath = FILE_MGR.Resolve("@EngineConfig/DefaultShaders.json");
    if (!ResourceLoader::Load(shaderConfigPath))
    {
        LOG_ERROR("ResourceManager: Failed to load Engine Shaders at '{}'", shaderConfigPath);
        return false;
    }

    // 3. 게임 리소스 로드
    std::string gameConfigPath = FILE_MGR.Resolve("@GameConfig/Resources.json");
    if (!ResourceLoader::Load(gameConfigPath))
    {
        LOG_WARN("ResourceManager: Game resource config not found at '{}'. Skipping.", gameConfigPath);
    }

    // =============================================================
    // [New Zone] Data-Driven Auto Scan 방식
    // =============================================================
    LOG_INFO("ResourceManager: Starting Auto-Scan Process...");

    LoadAssetConfig();
    LoadAllInDirectory("@Engine");
    LoadAllInDirectory("@Game");

    return true;
}

void ResourceManager::Clear()
{
	m_resources.clear();
    m_extToTypeMap.clear();
	LOG_INFO("ResourceManager Cleared.");
}

/*=======================================//
//   Data-driven resource loading test   //
//=======================================*/
void ResourceManager::LoadAssetConfig()
{
    std::string configPath = FILE_MGR.Resolve("@EngineConfig/AssetConfig.json");
    std::ifstream file(configPath);

    if (!file.is_open())
    {
        // 아직 설정 파일이 없을 수도 있으니 에러보다는 경고/정보
        LOG_WARN("ResourceManager: AssetConfig.json not found at '{}'. Auto-scan mapping will be empty.", configPath);
        return;
    }

    // 1. 파싱 (No Exception)
    json root = json::parse(file, nullptr, false);
    if (root.is_discarded())
    {
        LOG_ERROR("ResourceManager: AssetConfig JSON Syntax Error.");
        return;
    }

    // 2. 매크로를 이용한 자동 변환 및 맵 변환
    AssetConfigData config = root.get<AssetConfigData>();
    for (const auto& [typeStr, extList] : config.mappings)
    {
        json tempJson = typeStr;
        AssetType assetType = tempJson.get<AssetType>();
        if (assetType == AssetType::Unknown) continue;
        for (const auto& ext : extList)
        {
            std::string lowerExt = ext;
            std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
            m_extToTypeMap[lowerExt] = assetType;
        }
    }

    LOG_INFO("ResourceManager: Asset Config Loaded. {} extensions mapped.", m_extToTypeMap.size());
}

void ResourceManager::LoadAllInDirectory(const std::string& virtualPath)
{
    std::string absPathStr = FILE_MGR.Resolve(virtualPath);
    fs::path rootPath(absPathStr);
    
    if (!fs::exists(rootPath)) return;

    // 재귀적 순회
    for (const auto& entry : fs::recursive_directory_iterator(rootPath))
    {
        if (entry.is_regular_file())
        {
            ImportResource(entry.path());
        }
    }
}

void ResourceManager::ImportResource(const fs::path& filePath)
{
    // 1. 확장자 추출
    std::string ext = filePath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 2. 등록된 확장자인지 확인 (Unknown이거나 맵에 없으면 스킵)
    if (m_extToTypeMap.find(ext) == m_extToTypeMap.end()) return;
    AssetType type = m_extToTypeMap[ext];
    std::string key = filePath.stem().string(); // 파일명을 키로 사용 (나중엔 경로 포함 고려)
    std::string pathStr = filePath.string();

    bool success = false;
    
    // 3. 타입별 로딩 분기
    switch (type)
    {
        case AssetType::Texture:
        {
            LOG_INFO("[Scanner] Found Texture: '{}' -> Key: [{}]", pathStr, key);
            // TODO: 여기서 .meta 파일 확인 로직 추가 가능 (MetaParser)
            /*auto texture = TextureUtils::LoadTexture(pathStr);
            if (texture) 
            {
                AddResource<Texture>(std::move(texture), key, pathStr);
                success = true;
            }*/
            break;
        }
        case AssetType::Model:
        {
            LOG_INFO("[Scanner] Found Model: '{}' -> Key: [{}]", pathStr, key);
            /*auto model = Model::Load(pathStr);
            if (model) {
                AddResource<Model>(std::move(model), key, pathStr);
                success = true;
            }*/
            break;
        }
        case AssetType::Animation:
        {
            LOG_INFO("[Scanner] Found Animation: '{}' -> Key: [{}]", pathStr, key);
            /*auto anim = Animation::Load(pathStr);
            if (anim) {
                AddResource<Animation>(std::move(anim), key, pathStr);
                success = true;
            }*/
            break;
        }
        case AssetType::Audio:
        {
            LOG_INFO("[Scanner] Found Audio: '{}' -> Key: [{}]", pathStr, key);
            //// 일단 SFX로 통일 (추후 메타데이터로 BGM 구분)
            //auto clip = AudioClip::LoadSFX(pathStr);
            //if (clip) {
            //    AddResource<AudioClip>(std::move(clip), key, pathStr);
            //    success = true;
            //}
            break;
        }
        default:
            break;
    }

    if (success)
    {
        LOG_INFO("Auto-Imported: [{}] ({})", key, ext);
    }
}

