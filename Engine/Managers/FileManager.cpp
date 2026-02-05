#include "EnginePch.h"
#include "FileManager.h"
#include "Misc/EngineDir.h"
#include "Managers/FileManager.inl"

bool FileManager::Init()
{
	using namespace EnginePolicy;

	// 1. 실행 경로 기준 설정
	m_binPath = GetExecutablePath();
	LOG_INFO("FileManager: Initializing... Executable Root at [{}]", m_binPath.string());

	// 2. 부트 설정 파일 로드 (./Config/EngineConfig.json)
	fs::path bootConfigPath = m_binPath / fs::path(Disk::BootConfigPath);
	if (!fs::exists(bootConfigPath)) return false;

	std::ifstream file(bootConfigPath);
	if (!file.is_open()) return false;

	// 3. JSON 파싱
	json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded()) return false;

	// 4. 스키마 검증 및 데이터 추출 (contains 사용)
	const std::string settingsKey(Schema::SettingsSection);
	if (!data.contains(settingsKey)) return false;

	// 4-1. 스키마 키 점검
	const auto& settings = data[settingsKey];
	const std::string kAssetRoot(Schema::KeyAssetRoot);
	const std::string kConfigRoot(Schema::KeyConfigRoot);
	const std::string kEngineDir(Schema::KeyEngineDir);
	const std::string kGameDir(Schema::KeyGameDir);

	// 5. 물리적 기준 경로 확정
	m_assetRoot = m_binPath / settings[kAssetRoot].get<std::string>();
	m_configRoot = m_binPath / settings[kConfigRoot].get<std::string>();

	// 6. 가상 경로 매핑 테이블 구축 (Vector에 넣기)
	std::string engineDirName = settings[kEngineDir].get<std::string>();
	std::string gameDirName = settings[kGameDir].get<std::string>();
	m_virtualPaths.push_back({ Path::PrefixAssets,       m_assetRoot });
	m_virtualPaths.push_back({ Path::PrefixEngine,       m_assetRoot / engineDirName });
	m_virtualPaths.push_back({ Path::PrefixGame,         m_assetRoot / gameDirName });
	m_virtualPaths.push_back({ Path::PrefixConfig,       m_configRoot });
	m_virtualPaths.push_back({ Path::PrefixEngineConfig, m_configRoot / engineDirName });
	m_virtualPaths.push_back({ Path::PrefixGameConfig,   m_configRoot / gameDirName });
	m_virtualPaths.push_back({ "@Shader", m_assetRoot / engineDirName / "Shaders" });
	
	// 7. 긴 문자열이 먼저 오도록 정렬
	std::sort(m_virtualPaths.begin(), m_virtualPaths.end(),
		[](const auto& a, const auto& b) {
			return a.first.length() > b.first.length();
		});

	// 8. 디버그 출력 (이제 m_virtualPaths가 있으니 에러 안 남)
	LOG_INFO("=== Virtual Path Mappings (Sorted) ===");
	for (const auto& [prefix, path] : m_virtualPaths)
	{
		LOG_INFO("  Key: [{}] -> Val: [{}]", prefix, path.string());
	}

	return true;
}

// TODO : 이건 이후에 다시 수정을 하긴 해야함. 안정적이지 못하고 비효율적임.
std::string FileManager::Resolve(const std::string& virtualPath)
{
	// 정렬된 리스트를 순회 (긴 것부터 검사하므로 안전함)
	for (const auto& [prefix, physicalPath] : m_virtualPaths)
	{
		if (virtualPath.compare(0, prefix.length(), prefix) == 0)
		{
			// 안전장치: 정확히 일치하거나, 그 뒤가 슬래시여야 함
			if (virtualPath.length() == prefix.length() ||
				virtualPath[prefix.length()] == '/' ||
				virtualPath[prefix.length()] == '\\')
			{
				std::string subPath = virtualPath.substr(prefix.length());
				fs::path result = physicalPath / fs::path(subPath).relative_path();
				return result.string();
			}
		}
	}
	return virtualPath;
}