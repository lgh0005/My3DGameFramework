#include "CorePch.h"
#include "AssetManager.h"
#include "Identity/MGFType.h"
#include "Managers/TaskManager.h"
#include "Managers/PathManager.h"
#include "Parsers/JsonParser.h"

namespace MGF3D
{
	AssetManager::AssetManager() = default;
	AssetManager::~AssetManager() = default;

	bool AssetManager::Init()
	{
		// 에셋 확장명을 파싱한다
		if (!LoadExtensionMap())
		{
			MGF_LOG_ERROR("AssetManager: Failed to load FileExtension.json");
			return false;
		}

		return true;
	}

	void AssetManager::Update()
	{
		// 1. 메인 스레드 전용 로컬 바구니
		SVector<AssetPtr> readyToCommit;

		{
			// 2. 최소화한 임계 영역(Critical Section) : 포인터 교체
			MGF_LOCK_SCOPE(m_commitMutex);
			if (m_commitQueue.Empty()) return;
			readyToCommit.Swap(m_commitQueue);
		}

		// 3. 이제 락이 없는 상태에서 마음껏 OpenGL API를 호출합니다.
		for (auto& res : readyToCommit)
		{
			if (!res) continue;

			// OnCommit은 반드시 메인 스레드(GL Context)에서 실행됨이 보장됩니다.
			if (res->OnCommit())
			{
				res->SetState(WaitableObjectState::Ready);
				res->OnRelease();
				MGF_LOG_INFO("AssetManager: '{}' is now Ready.", res->GetName().CStr());
			}
			else
			{
				res->SetState(WaitableObjectState::Failed);
				MGF_LOG_ERROR("AssetManager: Failed to commit '{}'.", res->GetName().CStr());
			}
		}
	}

	void AssetManager::Shutdown()
	{
		MGF_LOG_INFO("AssetManager: Shutting down.");

		m_assets.Release();
		m_loaders.Release();
	}

	void AssetManager::RegisterLoader(const Ptr<MGFType> type, IAssetImporterUPtr loader)
	{
		if (!loader) return;

		if (m_loaders.Find(type))
		{
			MGF_LOG_WARN("AssetManager: Loader for '{}' already registered. Overwriting...", type->name.CStr());
			m_loaders.Remove(type);
		}

		m_loaders.Insert(type, std::move(loader));
	}

	void AssetManager::AddAsset(const AssetPtr& asset)
	{
		if (!asset) return;

		MGF_LOCK_SCOPE(m_commitMutex);
		StringHash key = asset->GetName().GetStringHash();
		if (m_assets.Find(key))
		{
			MGF_LOG_WARN("AssetManager: Resource '{}' already exists in cache.", asset->GetName().CStr());
			return;
		}

		m_assets.Insert(key, asset);
	}

	AssetPtr AssetManager::LoadAssetAsync(IAssetDescriptorUPtr desc)
	{
		if (!desc) return nullptr;

		// 1. 적절한 로더 찾기
		auto loaderPtr = m_loaders.Find(desc->type);
		if (!loaderPtr)
		{
			MGF_LOG_ERROR("AssetManager: No loader for TypeID {:x}", desc->type->name.CStr());
			return nullptr;
		}

		// [주의] 텍스쳐와 같은 OpenGL 컨텍스트에 종속된 작업은
		// 이쪽으로 넘기면 절대로 안된다. 순수 CPU 바운드 입출력을 요구하는
		// 무거운 작업을 이쪽으로 넘겨야 한다.
		// ex) 나쁜 예시: Texture, CubeTexture 등은 이쪽이 아닌 OnCommit으로,
		//	   좋은 예시: Material 로드를 위한 파싱, 모델 또는 애니메이션 로드를 위한 파싱 등
	
		// 2. 에셋 객체(껍데기) 생성
		// 로더의 Load 함수가 내부적으로 Asset 자식 객체를 '생성'만 하도록 설계되어 있어야 합니다.
		AssetPtr newAsset = nullptr;

		// 임시 Descriptor 전달을 위한 처리 (소유권 주의)
		if (!(*loaderPtr)->Load(newAsset, std::move(desc)))
			return nullptr;

		// 3. 상태를 'Pending(대기)'으로 설정하고 캐시에 먼저 등록
		newAsset->SetState(WaitableObjectState::Pending);
		AddAsset(newAsset);

		// 4. [비동기 시작] TaskManager에게 무거운 작업(OnLoad)을 던집니다.
		auto loadTask = MGF_TASK.AcquireTask
		(
			// [WORKER THREAD 영역]
			[this, newAsset]()
			{
				newAsset->SetState(WaitableObjectState::Loading);

				// 파일 I/O 및 파싱 (무거운 작업)
				if (newAsset->OnLoad())
				{
					newAsset->SetState(WaitableObjectState::WaitingCommit);
					MGF_LOCK_SCOPE(m_commitMutex);
					m_commitQueue.PushBack(newAsset);
				}
				else
				{
					MGF_LOG_ERROR("ResourceManager: Failed to OnLoad -> {}", newAsset->GetName().CStr());
					newAsset->SetState(WaitableObjectState::Failed);
				}
			},

			// [COMPLETION 콜백 (Main Thread)]
			[newAsset]()
			{
				MGF_LOG_INFO("ResourceManager: Async Load Task Finished for {}", newAsset->GetName().CStr());
			}
		);

		// 5. 워커 스레드 큐에 투입
		MGF_TASK.PushTask(std::move(loadTask));

		// 껍데기 포인터를 즉시 반환 (IsReady()는 아직 false인 상태)
		return newAsset;
	}
	
	bool AssetManager::LoadExtensionMap(const MGFPath& configFileName)
	{
		MGFPath extConfig = MGF_PATH.Resolve("@Config") / configFileName;

		JsonParser parser;
		if (!parser.LoadFromJsonFile(extConfig)) return false;

		const auto& root = parser.GetRoot();
		if (root.contains("extensionMap"))
		{
			for (auto& [ext, typeName] : root["extensionMap"].items())
			{
				m_extensionMap.Insert
				(
					StringHash(ext.c_str()),
					SString(typeName.get<str>().c_str())
				);
			}
		}

		MGF_LOG_INFO("AssetManager: Registered {} extension mapping pairs.", m_extensionMap.Count());
		return true;
	}
}