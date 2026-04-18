#include "CorePch.h"
#include "PathManager.h"
#include "Parsers/JsonParser.h"

namespace MGF3D
{
	PathManager::PathManager() = default;
	PathManager::~PathManager() = default;

	bool PathManager::Init(const String& configPath)
	{
		JsonParser parser;

		// 1. JSON 파일 로드
		if (!parser.LoadFromJsonFile(configPath))
		{
			MGF_LOG_ERROR("PathManager: Failed to load path configuration from '{}'", configPath);
			return false;
		}

		// 2. "virtualPaths" 오브젝트 탐색
		const auto& root = parser.GetRoot();
		const std::string sectionName = "virtualPaths";

		if (root.contains(sectionName) && root[sectionName].is_object())
		{
			const auto& virtualPathsObj = root[sectionName];

			// 3. 모든 가상 경로 엔트리 순회 및 등록
			for (auto& [alias, actualPath] : virtualPathsObj.items())
				RegisterVirtualPath(alias, actualPath.get<String>());
		}
		else
		{
			MGF_LOG_WARN("PathManager: 'virtualPaths' section not found in config.");
			return false;
		}

		MGF_LOG_INFO("PathManager: Virtual path system initialized with {} entries.", m_virtualPaths.size());
		return true;
	}

	void PathManager::RegisterVirtualPath(const String& alias, const String& actualPath)
	{
		LockScope lock(m_pathMutex);
		m_virtualPaths[StringHash(alias)] = actualPath;
	}

	String PathManager::ResolvePath(const String& virtualPath) const
	{
		// 1. 비어있거나 가상 경로 기호(@)로 시작하지 않으면 그대로 반환
		if (virtualPath.empty() || virtualPath[0] != '@')
			return virtualPath;

		// 2. 가상 경로 구분자('/') 위치 확인 (예: "@Assets/Textures/Logo.png")
		usize pos = virtualPath.find_first_of('/');

		// 경로의 끝까지가 별칭일 수도 있으므로 분리 기준을 정함
		String alias = CommonUtils::Select(pos == String::npos, virtualPath, virtualPath.substr(0, pos));
		String remainder = CommonUtils::Select(pos == String::npos, EMPTY_STRING, virtualPath.substr(pos));

		// 3. 등록된 별칭이 있는지 확인
		{
			LockScope lock(m_pathMutex);
			auto it = m_virtualPaths.find(StringHash(alias));
			if (it != m_virtualPaths.end()) return it->second + remainder;
		}

		MGF_LOG_WARN("PathManager: Failed to resolve virtual path '{}'", virtualPath);
		return virtualPath;
	}

}