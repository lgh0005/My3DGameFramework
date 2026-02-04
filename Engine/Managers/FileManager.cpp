#include "EnginePch.h"
#include "FileManager.h"
#include "Misc/EngineDir.h"

bool FileManager::Init()
{
	using namespace EnginePolicy;

	// 1. 실행 경로 기준 설정
	m_binPath = fs::current_path();

	// 2. 부트 설정 파일 로드 (./Config/EngineConfig.json)
	fs::path bootConfigPath = m_binPath / fs::path(Disk::BootConfigPath);
	if (!fs::exists(bootConfigPath))
	{
		LOG_ERROR("TODO");
		return false;
	}

	std::ifstream file(bootConfigPath);
	if (!file.is_open())
	{
		LOG_ERROR("TODO");
		return false;
	}

	// 3. JSON 파싱
	json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded())
	{
		LOG_ERROR("TODO");
		return false;
	}

	// 4. 스키마 검증 및 데이터 추출 (contains 사용)
	const std::string settingsKey(Schema::SettingsSection);
	if (!data.contains(settingsKey))
	{
		LOG_ERROR("TODO");
		return false;
	}

	// 4-1. 스키마 키 점검
	const auto& settings = data[settingsKey];
	const std::string kAssetRoot(Schema::KeyAssetRoot);
	const std::string kConfigRoot(Schema::KeyConfigRoot);
	const std::string kEngineDir(Schema::KeyEngineDir);
	const std::string kGameDir(Schema::KeyGameDir);
	if (!settings.contains(kAssetRoot) || !settings.contains(kConfigRoot) ||
		!settings.contains(kEngineDir) || !settings.contains(kGameDir))
	{
		LOG_ERROR("TODO");
		return false;
	}

	// 5. 물리적 기준 경로 확정
	m_assetRoot = m_binPath / settings[kAssetRoot].get<std::string>();
	m_configRoot = m_binPath / settings[kConfigRoot].get<std::string>();

	// 6. 가상 경로 매핑 테이블 구축
	const std::string pEngine(Path::PrefixEngine);
	const std::string pGame(Path::PrefixGame);
	const std::string pAssets(Path::PrefixAssets);
	const std::string pConfig(Path::PrefixConfig);
	const std::string pEngineConfig(Path::PrefixEngineConfig);
	const std::string pGameConfig(Path::PrefixGameConfig);

	std::string engineDirName = settings[kEngineDir].get<std::string>();
	std::string gameDirName = settings[kGameDir].get<std::string>();

	m_pathMap[pAssets] = m_assetRoot;
	m_pathMap[pEngine] = m_assetRoot / engineDirName;
	m_pathMap[pGame] = m_assetRoot / gameDirName;

	m_pathMap[pConfig] = m_configRoot;
	m_pathMap[pEngineConfig] = m_configRoot / engineDirName;
	m_pathMap[pGameConfig] = m_configRoot / gameDirName;

	// 7. 엔진 필수 폴더 유효성 검사
	if (!fs::exists(m_pathMap[pEngine]))
	{
		LOG_ERROR("TODO");
		return false;
	}

	return true;
}

std::string FileManager::Resolve(const std::string& virtualPath)
{
	// 가상 경로 접두사 매칭 확인
	for (const auto& [prefix, physicalPath] : m_pathMap)
	{
		if (virtualPath.compare(0, prefix.length(), prefix) == 0)
		{
			std::string subPath = virtualPath.substr(prefix.length());
			fs::path result = physicalPath / fs::path(subPath).relative_path();
			return result.string();
		}
	}

	return virtualPath;
}