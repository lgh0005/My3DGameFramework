#include "EnginePch.h"
#include "FileManager.h"
#include "Managers/FileManager.inl"

bool FileManager::Init()
{
	// 1. 실행 경로 기준 설정
	m_binPath = GetExecutablePath();
	LOG_INFO("FileManager: Initializing... Executable Root at [{}]", m_binPath.string());

	// 2. 부트 설정 파일 로드
	fs::path bootConfigPath = m_binPath / "Config/EngineConfig.json";
	std::ifstream file(bootConfigPath);
	if (!file.is_open()) return false;

	// 3. JSON 파싱
	json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded()) return false;

	// 4. 가상 경로 매핑 테이블 구축
	if (data.contains("virtualPaths"))
	{
		for (auto& [key, val] : data["virtualPaths"].items())
		{
			std::string relativePathStr = val.get<std::string>();
			fs::path absPath = m_binPath / relativePathStr;
			m_virtualPathMap[key] = absPath;
		}
	}

	if (m_virtualPathMap.count("@Assets")) m_assetRoot = m_virtualPathMap["@Assets"];
	if (m_virtualPathMap.count("@Config")) m_configRoot = m_virtualPathMap["@Config"];

	// 5. 디버그 출력
	LOG_INFO("=== Virtual Path Mappings (Hash Map) ===");
	for (const auto& [key, path] : m_virtualPathMap)
		LOG_INFO("  [{}] -> [{}]", key, path.string());

	return true;
}

/*==================================//
//   file path resolution methods   //
//==================================*/
std::string FileManager::Resolve(const std::string& virtualPath)
{
	// 1. 빈 경로 체크
	if (virtualPath.empty()) return "";

	// 2. 가상 경로(@) 처리 로직
	if (IsVirtualPath(virtualPath))
		return ResolveVirtualPath(virtualPath);

	// 3. 가상 경로가 아닌 경우 처리
	return ResolveAbsolutePath(virtualPath);
}

bool FileManager::IsVirtualPath(const std::string& path)
{
	// 1. 기본 체크: 비어있거나 @로 시작하지 않으면 탈락
	if (path.empty() || path[0] != '@')
		return false;

	// 2. 최소 길이 체크
	// "@" 한 글자만 있는 경우는 유효한 경로가 아님
	if (path.length() < 2)
		return false;

	// 3. "@\", "@/"로 시작한다면 가상 경로가 아님
	char secondChar = path[1];
	if (secondChar == '/' || secondChar == '\\')
		return false;

	return true;
}

std::string FileManager::ResolveVirtualPath(const std::string& path)
{
	usize slashPos = path.find_first_of("/\\");
	std::string key;
	std::string subPath;
	if (slashPos == std::string::npos) key = path;
	else
	{
		key = path.substr(0, slashPos);
		subPath = path.substr(slashPos + 1);
	}

	auto it = m_virtualPathMap.find(key);
	if (it != m_virtualPathMap.end())
	{
		// 찾음: 절대 경로 + 하위 경로 결합
		fs::path result = it->second;
		if (!subPath.empty()) result /= subPath;
		return result.string();
	}

	LOG_ERROR("FileManager: Unregistered virtual path key '{}'", key);
	return "";
}

std::string FileManager::ResolveAbsolutePath(const std::string& path)
{
	fs::path fsPath(path);

	// 1. 이미 절대 경로인 경우
	if (fsPath.is_absolute())
		return fsPath.make_preferred().string();

	// 2. 상대 경로인 경우
	fs::path absPath = m_binPath / fsPath;

	// 3. 경로 정규화
	return absPath.lexically_normal().make_preferred().string();
}
