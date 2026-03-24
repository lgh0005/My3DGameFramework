#include "CorePch.h"
#include "PathManager.h"
#include "Parsers/JsonParser.h"
#include "Managers/StreamManager.h"

namespace MGF3D
{
	PathManager::PathManager() = default;
	PathManager::~PathManager() = default;

	bool PathManager::Init()
	{
		// 1. 별도의 경로 계산 없이, 실행 파일이 있는 위치를 기준으로 한 상대 경로
		AddVirtualPath("@Config", "Config");

		// 2. 비동기 EngineConfig.json 파싱
		if (!LoadConfig()) return false;
		WaitForConfig();

		MGF_LOG_INFO("PathManager: All configurations loaded and verified.");
		return true;
	}

	void PathManager::Shutdown()
	{
		MGF_LOCK_SCOPE(m_pathMutex);
		m_virtualPaths.Release();
		MGF_LOG_INFO("PathManager: Shutdown successfully.");
	}

	bool PathManager::LoadConfig(const MGFPath& configFileName)
	{
		// 2. Config 파일을 불러오기
		MGFPath configPath = Resolve("@Config") / configFileName;
		if (!configPath.Exists())
		{
			MGF_LOG_ERROR("PathManager: Config file not found: {}", configPath.GetCStr());
			return false;
		}

		// StreamManager를 통해 비동기로 읽기 시작
		MGF_STREAM.ReadFileAsync<bool>
		(
			configPath,

			/* Processor */
			[this](FileStreamPtr file) -> Nullable<bool>
			{
				JsonParser parser;

				// FileStream에서 직접 데이터를 읽으며 파싱
				if (!parser.LoadFromStream(file))
					return None;

				const auto& root = parser.GetRoot();
				if (root.contains("virtualPaths"))
				{
					const auto& pathNode = root["virtualPaths"];
					for (auto& [alias, physical] : pathNode.items())
					{
						// 아까 확인한 대로 내부에서 락을 걸어주니 안전합니다!
						AddVirtualPath(alias.c_str(), physical.get<std::string>().c_str());
					}
				}

				return true;
			},

			/* On Complete */
			[this](Nullable<bool> result)
			{
				if (result.IsValid() && *result)
				{
					m_configSignal.Set();
					MGF_LOG_INFO("PathManager: Config stream-parsed and registered in background.");
				}
			}
		);

		return true;
	}

	void PathManager::AddVirtualPath(const MGFName& alias, const MGFPath& actualPath)
	{
		MGF_LOCK_SCOPE(m_pathMutex);

		// MGFName에서 StringHash를 꺼내서 키로 사용합니다.
		StringHash key = alias.GetStringHash();

		if (m_virtualPaths.Find(key))
		{
			MGF_LOG_WARN("PathManager: Alias already exists. Overwriting...");
			m_virtualPaths.Remove(key);
		}

		m_virtualPaths.Insert(key, actualPath);
	}

	MGFPath PathManager::Resolve(const MGFPath& virtualPath) const
	{
		const SString& raw = virtualPath.ToSString();

		// 0. 비어있거나 @로 시작하지 않으면 가상 경로가 아님
		if (raw.Empty() || raw.CStr()[0] != '@')
			return virtualPath;

		// 1. fs::path로 변환 및 가상 경로 토큰(Alias) 획득
		// MGFPath가 내부적으로 fs::path를 들고 있으므로 캐스팅을 활용합니다.
		const fs::path& p = (const fs::path&)virtualPath;
		auto it = p.begin();

		// 첫 번째 토큰이 Alias입니다. (@Assets 등)
		StringHash aliasHash(it->string().c_str());

		// 2. 가상 경로 매핑 확인
		MGFPath resolvedBase;
		{
			MGF_LOCK_SCOPE(m_pathMutex);

			auto ptr = m_virtualPaths.Find(aliasHash);
			if (!ptr) return virtualPath;
			resolvedBase = *ptr;
		}

		// 3. 매핑된 실제 경로로 시작하는 결과 경로 생성
		fs::path resolvedPath = (const fs::path&)resolvedBase;

		// 4. 나머지 조각들을 순차적으로 결합
		for (++it; it != p.end(); ++it)
			resolvedPath /= *it;

		// 5. 정문화(lexically_normal) 및 반환
		return MGFPath(resolvedPath.lexically_normal());
	}

	void PathManager::WaitForConfig()
	{
		m_configSignal.Wait();
	}
}